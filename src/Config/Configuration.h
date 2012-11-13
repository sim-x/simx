//--------------------------------------------------------------------------
// $Id: Configuration.h,v 1.1.1.1 2011/08/18 22:19:44 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    Configuration.h
// Module:  Config
// Author:  Paula Stretz, Sunil Thulasidasan
// Created:
// Description: 
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_CONFIG_CONFIGURATION
#define NISAC_CONFIG_CONFIGURATION

#include "simx/Common/Exception.h"
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

//--------------------------------------------------------------------------

/// Classes for operating on configuration files.

namespace Config {  

  //--------------------------------------------------------------------------

  class Configuration;
  class ConfigurationFileReader;
  extern Configuration gConfig;

  //--------------------------------------------------------------------------

  /// \brief Initialize the global gConfig from a file.
  void ConfigInit(const std::string& confgFilename);

  /// Initialize gConfig without a file
  void ConfigInit();


  /// \class Configuration Configuration.h "Config/Configuration.h"
  ///
  /// \brief Read and provide access to a NISAC configuration file
  ///
  ///  Only the last reading of a key is remembered.  Any earlier
  ///  readings of that key are forgotten.

  class Configuration
  {

  public:

    typedef std::less<const std::string> LessConstString;
    typedef std::map<std::string, std::string, LessConstString> ConfigMap;
    typedef ConfigMap::iterator ConfigMapIterator;
    typedef ConfigMap::const_iterator ConfigMapConstIterator;
    typedef std::map<std::string, std::string> VarMap;
    typedef std::pair<std::string, ConfigMap*> ConfigSubset;
    typedef std::set<ConfigSubset> ConfigSet;
    typedef std::multimap<std::string, ConfigSubset, LessConstString> 
    ConfigSubsetMap;

    Configuration ();
    Configuration (int);
    Configuration (ConfigurationFileReader&);
    Configuration (const Configuration&);

    ~Configuration();

    Configuration& operator=(const Configuration&);

    bool operator==(const Configuration&) const;
    bool operator!=(const Configuration&) const;

    /// Return a config value as the specified type.  The type must be
    /// able to be initialized from a stream.
    template <typename T>
    bool GetConfigurationValue
    (const std::string& key, T&, const ConfigMap* cmap = 0) const;

    /// Return a config value as the specified type.  The type must be
    /// able to be initialized from a stream.
    template <typename T>
    bool GetConfigurationValue
    (const char* key, T&, const ConfigMap* cmap = 0) const;

    /// Return a configuration attribute. Throw an exception if
    /// the key is not found.
    template <typename T>
    void GetConfigurationValueRequired
    (const std::string&, T&, const ConfigMap* cmap = 0) const;

    /// Return a configuration attribute. Throw an exception if
    /// the key is not found.
    template <typename T>
    void GetConfigurationValueRequired
    (const char* key, T& v, const ConfigMap* cmap = 0) const;

    /// Provide a default value to use if the key is not found
    template <typename T>
    void GetConfigurationValue
    (const std::string& key, T& v, T default_v, 
     const ConfigMap* cmap = 0) const;

    /// Provide adefault value to use if the key is not found
    template <typename T>
    void GetConfigurationValue
    (const char* key, T& v, T default_v,
     const ConfigMap* cmap = 0) const;

    /// Return the value of the variable
    std::string GetVariable(const std::string& name) const;

    ///  Return whether a configuration attribute is defined
    bool IsBound(const std::string&, 
		 const ConfigMap* cmap = 0) const;

    ///  Return the entire configuration map
    ConfigMap& GetConfiguration(ConfigMap* cmap = 0);

    ///  Return the entire configuration map
    const ConfigMap& GetConfiguration(const ConfigMap* cmap = 0) const;

    ///  Return the configuration associated with the named set
    ConfigSet GetConfigurationSet(const std::string& name) const;

    ///  Return the configuration associated with the named set
    ConfigSet GetConfigurationSet(const char* name) const;

    ///  Return all the configuration sets
    const ConfigSubsetMap& GetConfigurationSubsets() const;

    //  Define a configuration attribute 
    void SetConfigurationValue (const std::string&, const std::string&, 
				ConfigMap* cmap = 0);
    // create a configuration set
    void createConfigurationSet(const std::string&, const std::string&,
				const std::map<std::string,std::string>& config );
    

  private:

    // Return true if the name is an include statement
    bool IsInclude(const std::string& name) const;

    // Return true if the name is a variable
    bool IsVariable(const std::string& name) const;

    // Extract a variable name from a string
    std::string ExtractVariableName(const std::string& name) const;

    // True if setting the debug log
    bool IsDebugLog(const std::string& name) const;

    // Set variable in name to value
    void SetVariable(const std::string& name, const std::string& value);

    

    // Process an include file.
    void Process(ConfigurationFileReader& reader, int depth);

    // Set predefined variables
    void SetDefaultVariables();

    //  Return true if name is a set statement
    bool IsSet(const std::string& name) const;

    //  Return true if name is {
    bool IsOpenBrace(const std::string& name) const;

    //  Return true if name is }
    bool IsCloseBrace(const std::string& name) const;

    //  Configuration is stored as attribute-value pairs
    ConfigMap fConfiguration;

    //  Variables are stored as name-value pairs
    VarMap fVariable;

    //  Sets of configuration parameters are stored in separate maps
    //  associated with set names
    ConfigSubsetMap fConfigSets;

    // File to record debugging information to
    std::ofstream fDebug;
  };


  //--------------------------------------------------------------------------

  // Return a config value as the specified type.  The type must be
  // able to be initialized from a stream.
  template <typename T>
  inline bool Configuration::GetConfigurationValue
  (const std::string& key, T& t, const ConfigMap* cmap) const
  {
    ConfigMap::const_iterator pos;
    ConfigMap::const_iterator end;
    if (cmap == 0) {
      pos = fConfiguration.find(key); 
      end = fConfiguration.end();
    } else {
      pos = cmap->find(key); 
      end = cmap->end();
    }
    if (pos == end)
      return false;
    std::stringstream str;
    if (pos->second == "")
    {
      t = T();
    }
    else
    {
      str << pos->second;
      str >> t;
    }
    return true;
  }

  //--------------------------------------------------------------------------

  // Specialize for the case that the value is a string.  Simply
  // return a copy of the value.
  template <>
  inline bool Configuration::GetConfigurationValue
  (const std::string& key, std::string& t, const ConfigMap* cmap) const
  {
    ConfigMap::const_iterator pos; 
    ConfigMap::const_iterator end;
    if (cmap == 0) {
      pos = fConfiguration.find(key); 
      end = fConfiguration.end();
    } else {
      pos = cmap->find(key); 
      end = cmap->end();
    }
    if (pos == end)
      return false;
    else
      t =  pos->second;
    return true;
  }

  //--------------------------------------------------------------------------

  // Specialize for the case that the value is a bool.  
  template<> 
  inline bool Configuration::GetConfigurationValue
  (const std::string& key, bool& t, const ConfigMap* cmap) const
  {
    ConfigMap::const_iterator pos;
    ConfigMap::const_iterator end;
    if (cmap == 0) {
      pos = fConfiguration.find(key); 
      end = fConfiguration.end();
    } else {
      pos = cmap->find(key); 
      end = cmap->end();
    }
    if (pos == end)
      return false;
    /// true is "1" or "true", everything else is false
    t = (pos->second == "1" || pos->second == "true");
    return true;
  }

  //--------------------------------------------------------------------------

  // Overload for the case that the key name is a char*
  template <typename T>
  inline bool Configuration::GetConfigurationValue
  (const char* key, T& t, const ConfigMap* cmap) const
  {
    return GetConfigurationValue(std::string(key), t, cmap);
  }

  //--------------------------------------------------------------------------

  // Overload for including a default value.
  template <typename T>
  inline void Configuration::GetConfigurationValue
  (const std::string& key, T& v, T default_v, const ConfigMap* cmap) const
  {
    if (!GetConfigurationValue(key, v, cmap))
      v = default_v;
  }

  //--------------------------------------------------------------------------

  // Overload for including a default value.
  template <typename T>
  inline void Configuration::GetConfigurationValue
  (const char* key, T& v, T default_v, const ConfigMap* cmap) const
  {
    GetConfigurationValue(std::string(key), v, default_v, cmap);
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline void Configuration::GetConfigurationValueRequired
  (const std::string& key, T& t, const ConfigMap* cmap) const
  {
    if (!GetConfigurationValue(key, t, cmap))
    {
      std::string msg("Configuration key ");
      msg += key + " is required.";
      throw Common::Exception(msg);
    }
  }

  //--------------------------------------------------------------------------

  template <typename T>
  inline void Configuration::GetConfigurationValueRequired
  (const char* key, T& t, const ConfigMap* cmap) const
  {
    GetConfigurationValueRequired(std::string(key), t, cmap);
  }

  //--------------------------------------------------------------------------
  //--------------------------------------------------------------------------

  template <typename T>
  bool GetList
  (const std::string& sep,const std::string& src,std::vector<T>& res )
  {
    using boost::lexical_cast;
    using boost::bad_lexical_cast;
    bool success = true;
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sepa(sep.c_str());
    tokenizer tokens(src,sepa);
    if (tokens.begin() == tokens.end()) {
      return(false);
    }
    for (tokenizer::iterator tok_iter = tokens.begin(); 
	 tok_iter != tokens.end(); ++tok_iter) {
      try {
	T item = lexical_cast<T>(*tok_iter);
	res.push_back(item);
      }
      catch (bad_lexical_cast &) {
	T item;
	res.push_back(item);
	success = false;
      }
    }
    return(success);
  }

  //--------------------------------------------------------------------------

  /// Return a string with all variable names substituted for values
  std::string Substitute
  (const std::string& val, const Configuration::ConfigMap *cmap = 0);

} // namespace

//--------------------------------------------------------------------------
#endif // NISAC_CONFIG_CONFIGURATION
//--------------------------------------------------------------------------
