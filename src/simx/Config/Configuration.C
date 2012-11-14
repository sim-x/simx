// Copyright (c) 2012. Los Alamos National Security, LLC. 

// This material was produced under U.S. Government contract DE-AC52-06NA25396
// for Los Alamos National Laboratory (LANL), which is operated by Los Alamos 
// National Security, LLC for the U.S. Department of Energy. The U.S. Government 
// has rights to use, reproduce, and distribute this software.  

// NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, 
// EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  
// If software is modified to produce derivative works, such modified software should
// be clearly marked, so as not to confuse it with the version available from LANL.

// Additionally, this library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License v 2.1 as published by the 
// Free Software Foundation. Accordingly, this library is distributed in the hope that 
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See LICENSE.txt for more details.

//--------------------------------------------------------------------------
// $Id: Configuration.C,v 1.1.1.1 2011/08/18 22:19:44 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    Configuration.C
// Module:  Config
// Author:  Paula Stretz, Sunil Thulasidasan
// Created:
// Description: 
//
// @@
//
//--------------------------------------------------------------------------

#include "simx/Config/Configuration.h"
#include "simx/Config/ConfigurationFileReader.h"
#include "simx/Config/Value.h"
#include "simx/Common/Values.h"
#include "config.h"             // autoconf generated file
#include <iostream>
#include <sstream>
#include <unistd.h>

using namespace Config;
using namespace std;

//--------------------------------------------------------------------------

namespace Config {

  Configuration gConfig;

  //--------------------------------------------------------------------------

  // Construct and initialize global gConfig from a config file.
  void ConfigInit(const std::string& configFilename)
  {
    ConfigurationFileReader r(configFilename);
    Configuration c(r);
    gConfig=c;
  }


  // initialize configuration without an input file.
  void ConfigInit() {
    //Configuration c(*static_cast<ConfigurationFileReader*>(NULL));
    Configuration c(0);
    gConfig = c;
  }

  //--------------------------------------------------------------------------
  // Configuration class methods
  //--------------------------------------------------------------------------

  // Construct an empty configuration
  Configuration::Configuration ()
    : fConfiguration()
  {
    fDebug.exceptions(ios::failbit |
		      ios::badbit |
		      ios::eofbit);
  }

  // Construct an empty configuration but initialize
  Configuration::Configuration (int i) : fConfiguration() {
    if ( i == 0 )
      SetDefaultVariables();
  }
    

  //--------------------------------------------------------------------------

  //  Construct a configuration using the reader
  Configuration::Configuration (ConfigurationFileReader& reader)
    : fConfiguration()
  {
    SetDefaultVariables();
    // Process the file
    Process(reader, 0);

    // Print out the keys
    if (fDebug)
    {
      fDebug << "\nFinal Values" << endl;
      for (ConfigMap::const_iterator it = fConfiguration.begin();
	   it != fConfiguration.end();
	   ++it)
      {
	fDebug << it->first << " = " << it->second << endl;
      }
      fDebug << "\nFinal Variables" << endl;
      for (VarMap::const_iterator it = fVariable.begin();
	   it != fVariable.end();
	   ++it)
      {
	fDebug << it->first << " = " << it->second << endl;
      }
      fDebug << "\nFinal Values in sets" << endl;
      for (ConfigSubsetMap::const_iterator is = fConfigSets.begin();
	   is != fConfigSets.end();
	   ++is)
      {
	fDebug << "set " << is->first 
	       << " subset " << is->second.first
	       << endl;
	for (ConfigMap::const_iterator it = is->second.second->begin();
	     it != is->second.second->end();
	     ++it)
	{
	  fDebug << it->first << " = " << it->second << endl;
	}
      }
    }
    
  }

  //--------------------------------------------------------------------------

  // Setup predefined variables
  void Configuration::SetDefaultVariables()
  {
    const int bufsize( 256 );
    char hnbuf[bufsize];
    gethostname(hnbuf, bufsize);
    string hostname(hnbuf);
    fVariable[string("$HOSTNAME")] = hostname;
    string::size_type pos = hostname.find(".");
    if (pos != string::npos)
      hostname.erase(pos, hostname.length());
    fVariable[string("$SHORT_HOSTNAME")] = hostname;

    stringstream rankstr;
    rankstr << Common::Values::gRank();
    string rank;
    rankstr >> rank;
    fVariable[string("$RANK")] = rank;
    fVariable[string("$RANKSUFFIX")] = Common::Values::gRankSuffix();
  }

  //--------------------------------------------------------------------------

  // Process an include file.
  void Configuration::Process(ConfigurationFileReader& reader, int depth)
  {
    // Set indention according to include depth
    string tabs;
    for (int i=0; i < depth; i++)
      tabs += '\t';

    bool inSet = false;
    bool active = false;
    ConfigMap* cmap = 0;

    fDebug << tabs << "Opening file " << reader.GetFilename() << endl;
    while (reader.MoreRecords())
    {
      reader.GetNextRecord();
      if (!reader.IsComment())
      {
        string attr(reader.GetAttribute());
	//cout << "attr is " << attr << endl;
        string valuestr(reader.GetValue());
	//cout << "value is " << valuestr << endl;
        Value value(valuestr);
        // Should we do a set configuration value.
        // No for variable defines, includes
        bool doSet=true;
        if (inSet && !active)
          doSet = false;

        // True is we did a variable substitution
        bool isVar=false;

        if (IsDebugLog(attr))
        {
          // Make sure we clear any prevoius errors, like writing to
          // the stream before it was opened
          fDebug.clear();
          fDebug.open(value.GetValue().c_str());
          if (!fDebug)
          {
            cout << "Problem opening debug file: "
		 << value.GetValue() << endl;
          }

          fDebug << tabs
		 << "Opening file " << reader.GetFilename() << endl;          
        }
        if (IsInclude(attr))
        {
          fDebug << tabs 
		 << "Including file " << value.GetValue() << endl;
          ConfigurationFileReader incReader(value.GetValue());
          Process(incReader, depth+1);
          doSet=false;
        }
        else if (IsVariable(attr))
        {
          fDebug << tabs
		 << attr << " = " << value.GetValue() << endl;
          SetVariable(attr, value.GetValue());
          doSet=false;
        }
        else if (value.HasVariable())
        {
          string var = value.VariableName();
          fDebug << tabs
		 << attr << " = " << value.VariableName() << "[";
          value.ReplaceVariable(GetVariable(var));
          fDebug << value.GetExpandedValue() << "]" << endl;
          isVar=true;
        }
        else if (IsSet(attr))
        {
          doSet = false;
          string setname, subsetname, activ;
          std::istringstream ist(value.GetValue());
          ist >> setname >> subsetname >> activ;
          active = (activ == "active") ? true : false;
          fDebug << tabs
		 << "Reading set " << setname
		 << " subset " << subsetname
		 << " status " << activ << endl;
          if (active)
	    {
            cmap = new ConfigMap;
            fConfigSets.insert
	      (make_pair(setname, make_pair(subsetname, cmap)));
          }
        }
        else if (IsOpenBrace(attr))
        {
          doSet = false;
          inSet = true;
        }
        else if (IsCloseBrace(attr))
        {
          doSet = false;
          inSet = false;
          cmap = 0;
        }

        if (doSet)
        {
          if (!isVar)
            fDebug << tabs 
		   << attr << " = " << value.GetValue() << endl;
          if (!inSet)
            SetConfigurationValue(attr, value.GetExpandedValue());
          else
            SetConfigurationValue(attr, value.GetExpandedValue(), cmap);
        }
      }
    }
    fDebug << tabs << "Closing file " << reader.GetFilename() << endl;
  }

  //--------------------------------------------------------------------------

  Configuration::Configuration (const Configuration& c)
    : fConfiguration()
  {
    fConfiguration = c.fConfiguration;
    fVariable = c.fVariable;
    for (ConfigSubsetMap::const_iterator i = c.fConfigSets.begin();
	 i != c.fConfigSets.end(); ++i) {
      ConfigMap* cmap = new ConfigMap(*i->second.second);
      fConfigSets.insert
	(make_pair(i->first, make_pair(i->second.first, cmap)));
    }
  }

  //--------------------------------------------------------------------------

  //  Destruct a configuration
  Configuration::~Configuration()
  {
    if (fDebug.is_open())
      fDebug.close();
    for (ConfigSubsetMap::iterator i = fConfigSets.begin();
	 i != fConfigSets.end(); ++i)
      delete i->second.second;
  }

  //--------------------------------------------------------------------------

  //  Assign a configuration
  Configuration& Configuration::operator =(const Configuration& c)
  {
    if (this == &c) return *this;
    fConfiguration = c.fConfiguration;
    fVariable = c.fVariable;
    for (ConfigSubsetMap::const_iterator i = c.fConfigSets.begin();
	 i != c.fConfigSets.end(); ++i) {
      ConfigMap* cmap = new ConfigMap(*i->second.second);
      fConfigSets.insert
	(make_pair(i->first, make_pair(i->second.first, cmap)));
    }
    return *this;
  }

  //--------------------------------------------------------------------------

  //  Define a configuration attribute 
  void Configuration::SetConfigurationValue
  (const string& attr, const string& value,
   Configuration::ConfigMap* cmap)
  {
    if (cmap == 0)
      fConfiguration[attr] = value;
    else
      cmap->insert(std::make_pair(attr, value));
  }

  //--------------------------------------------------------------------------

  //  Return whether a configuration attribute is defined
  bool Configuration::IsBound
  (const string& attr, const ConfigMap* cmap) const
  {
    if (cmap == 0)
      return fConfiguration.find(attr) != fConfiguration.end();
    else
      return cmap->find(attr) != cmap->end();
  }

  //--------------------------------------------------------------------------

  //  Return the entire configuration map
  Configuration::ConfigMap& Configuration::GetConfiguration
  (ConfigMap* cmap)
  {
    if (cmap == 0)
      return fConfiguration;
    else
      return *cmap;
  }

  //--------------------------------------------------------------------------

  const Configuration::ConfigMap& Configuration::GetConfiguration
  (const ConfigMap* cmap) const
  {
    if (cmap == 0)
      return fConfiguration;
    else
      return *cmap;
  }

  //--------------------------------------------------------------------------
  
  void Configuration::createConfigurationSet(const string& set_name, 
					     const string& subset_name,
					     const map<string,string>& config ) {
    
    ConfigMap* cmap = new ConfigMap;
    fConfigSets.insert(make_pair(set_name, 
				 make_pair(subset_name, cmap)));
    map<string,string>::const_iterator iter;
    for (iter = config.begin(); iter != config.end(); ++iter )
      cmap->insert(*iter);
  }
    

  //--------------------------------------------------------------------------
  //  Return the configuration associated with the named set
  Configuration::ConfigSet Configuration::GetConfigurationSet
  (const std::string& name) const
  {
    Configuration::ConfigSet s;
    Configuration::ConfigSubsetMap::const_iterator i;
    for (i = fConfigSets.lower_bound(name);
	 i != fConfigSets.upper_bound(name); ++i)
      s.insert(i->second);
    return s;
  }

  //--------------------------------------------------------------------------

  Configuration::ConfigSet Configuration::GetConfigurationSet
  (const char* name) const
  {
    return GetConfigurationSet(string(name));
  }

  //--------------------------------------------------------------------------

  //  Return all the configuration sets
  const Configuration::ConfigSubsetMap& 
  Configuration::GetConfigurationSubsets() const
  {
    return fConfigSets;
  }

  //--------------------------------------------------------------------------

  // Return true if the name is an include statement
  bool Configuration::IsInclude(const std::string& name) const
  {
    return (name == "include");
  }

  //--------------------------------------------------------------------------

  // Return true if the name is a variable
  bool Configuration::IsVariable(const std::string& name) const
  {
    string::size_type pos = name.find("$");
    if (pos == string::npos)
      return false;
    return true;
    //    return (name.length() > 1 && name[0] == '$');
  }

  //--------------------------------------------------------------------------

  // Extract the name of the variable
  string Configuration::ExtractVariableName
  (const std::string& name) const
  {
    static char alphanum[] = 
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789{_";
    string::size_type begin = name.find_first_of("$");
    string::size_type end = name.find_first_not_of(alphanum, begin+1);

    string var;
    if (end == string::npos)
      var = name.substr(begin);
    else
      var = name.substr(begin, end-begin);
    if (var[1] == '{')
    {
      var.erase(1, 1);
    }
    cout << "ExtractVariableName: " << name << " "
	 << begin << " " << end << " "
	 << var << endl;
    return var;
  }

  //--------------------------------------------------------------------------

  // True if setting the debug log
  bool Configuration::IsDebugLog(const std::string& name) const
  {
    return name == "CONFIG_DEBUG_LOG";
  }

  //--------------------------------------------------------------------------

  // Set variable in name to value
  void Configuration::SetVariable(const string& name, const string& value)
  {
    fVariable[name] = value;
  }

  //--------------------------------------------------------------------------

  // Return the value of the variable
  string Configuration::GetVariable(const string& name) const
  {
    // Variable extends from start to first '/'
    string::size_type pos = name.find("/");
    string var = name.substr(0, pos);

    VarMap::const_iterator it = fVariable.find(var);
    if (it == fVariable.end())
      return "";
    else
    {
      // variable is entire name
      if (name == var)
        return it->second;
      // otherwise the variable is a prefix
      else
      {
        string value = name;
        value.replace(0, pos, it->second);
        return value;
      }
    }
  }

  //--------------------------------------------------------------------------

  //  Return true if name is a set statement
  bool Configuration::IsSet(const std::string& name) const
  {
    return name == "set";
  }

  //--------------------------------------------------------------------------

  //  Return true if name is {
  bool Configuration::IsOpenBrace(const std::string& name) const
  {
    return name == "{";
  }

  //--------------------------------------------------------------------------

  //  Return true if name is }
  bool Configuration::IsCloseBrace(const std::string& name) const
  {
    return name == "}";
  }

  //--------------------------------------------------------------------------

  std::string Substitute
  (const std::string& val, const Configuration::ConfigMap *cmap)
  {
    string::size_type i = val.find_first_of("$");
    string::size_type j = val.find_first_of("$ ",i+1);
    std::string ret = val;
    if (i == string::npos)
    {
      return(ret);
    }
    if (i == 0 || (i > 0 && val[i-1] != '\\') )
    {
      std::string varVal = gConfig.GetVariable(val.substr(i,j-i));
      ret.replace(i,j-i,varVal);
      return(Substitute(ret));
    }
    else if (i > 0)
    {
      return(ret.substr(0,i-1) + "$" + 
	     Substitute(ret.substr(i+1,ret.length()-i)) );
    }
    return( val.substr(0,i) +
	    Substitute(val.substr(i+1,val.length()-i)) );
  }

  //--------------------------------------------------------------------------

} // namespace

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
