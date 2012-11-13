//--------------------------------------------------------------------------
// $Id: FileReader.h,v 1.1.1.1 2011/08/18 22:19:46 nsanthi Exp $
//--------------------------------------------------------------------------
// File:    FileReader.h
// Module:  File
// Author:  Keith Bisset
// Created: July  1 2002
// Description: Data reader for all files in the NISAC file format.
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_FILE_FILEREADER
#define NISAC_FILE_FILEREADER
#include "simx/File/Header.h"
#include <boost/shared_ptr.hpp>
#include "simx/Common/Exception.h"
#include "simx/Log/Logger.h"
#include <string>
#include "simx/File/fstream.h"
#include <sstream>

//--------------------------------------------------------------------------

namespace File {
  /// How to handle comments
  enum ECommentAction {
    kCommentOn,			///< Check for and skip comments
    kCommentOff,		///< Don't check for comments
    kCommentToggle,		///< Switch how comments are handles
    kCommentStatus		///< Report current state of comment skipping
  };

  /// \class FileReader FileReader.h "File/FileReader.h"
  ///
  /// \brief Data reader for all files in the NISAC file format.
  ///
  template<class Data, int NumFixedFields>
  class FileReader
  {
  public:
    /// Default constructor
    FileReader();
    /// Create a reader for a particular file.
    /// Identical to FileReader r(); r.Open(filename);
    FileReader(const std::string& filename);
    ~FileReader();

    /// Open a file for reading.
    /// The first line of the file is read and parsed as a header.
    void Open(const std::string& filename);

    /// Close a file.
    void Close();

    /// Is there more data to be read?
    bool MoreData();

    /// Read a record from the file.
    /// Lines starting with '#' are ignored by default.
    Data ReadData();

    /// Provides read-only look into the data that's next to be read
    /// Does not read nything
    const Data& ViewNextData() const;

    /// Get the file header
    const Header& GetHeader() const;

    /// Return the name of the field in the index column
    const std::string& FieldName(int index);

    /// Change or query the comment skipping status.  Returns the new
    /// status.  The status defaults to on.
    bool Comment(ECommentAction action);

  protected:
  private:
    void ReadNextRecord();
    
    Data fNext;
    File::ifstream fFile;
    HeaderPtr fHeader;
    bool fMore;
    bool fComment;
  };

  //--------------------------------------------------------------------------

  extern HeaderPtr fgEmptyHeader;

  //--------------------------------------------------------------------------

  template<class Data, int NumFixedFields>
  inline bool FileReader<Data, NumFixedFields>::MoreData()
  {
    return fMore;
  }

  //--------------------------------------------------------------------------

  template<class Data, int NumFixedFields>
  inline FileReader<Data, NumFixedFields>::FileReader()
    : fHeader(fgEmptyHeader),
      fMore(false),
      fComment(true)
  {
  }

  //--------------------------------------------------------------------------

  template<class Data, int NumFixedFields>
  inline FileReader<Data, NumFixedFields>::
  FileReader(const std::string& filename)
    : fHeader(fgEmptyHeader),
      fMore(true),
      fComment(true)
  {
    Open(filename);
  }

  template<class Data, int NumFixedFields>
  inline FileReader<Data, NumFixedFields>::~FileReader()
  {
    fFile.close();
  }

  //--------------------------------------------------------------------------
    
  template<class Data, int NumFixedFields>
    inline void FileReader<Data, NumFixedFields>::
    Open(const std::string& filename)
  {
    fHeader = HeaderPtr(new Header(NumFixedFields));

    fFile.open(filename);
    std::string sline;
    getline(fFile, sline);
    fMore = (sline.length() > 0);
    if (fMore)
    {
      if( sline[0] == '#' )
      {
	sline[0] = ' ';
      } else
      {
	Log::log().warn(0) 
	  << "Header line doesn't start with comment in file "
	  << filename << std::endl;
      }
      std::stringstream line(sline);
      std::ws(line);
      line >> *fHeader;
    } else {
      std::string msg("Couldn't read header from file ");
      msg += filename;
      throw Common::Exception(msg);
    }

    //simple warning prints out once for each header read
    if(NumFixedFields > fHeader->NumberFields())
      Log::log().warn(0)
	<< "Your template specifies " 
	<< NumFixedFields << " fields,"
	<< " when in reality there are "
	<< fHeader->NumberFields()
	<< " in " << filename << std::endl;
  
    fNext.SetHeader(fHeader);
    ReadNextRecord();
  }

  //--------------------------------------------------------------------------

  template<class Data, int NumFixedFields>
  inline void FileReader<Data, NumFixedFields>::Close()
  {
    fMore = false;
    fHeader = fgEmptyHeader;
    if (fFile.is_open())
    {
      fFile.close();
      fFile.clear();
    }
  }

  //--------------------------------------------------------------------------
  
  template<class Data, int NumFixedFields>
  inline const Header& FileReader<Data, NumFixedFields>::GetHeader() const
  {
    return *fHeader;
  }

  //--------------------------------------------------------------------------

  template<class Data, int NumFixedFields>
    inline const std::string& FileReader<Data, NumFixedFields>::
    FieldName(int index)
  {
    return fHeader->GetFieldName(index);
  }

  //--------------------------------------------------------------------------

  template<class Data, int NumFixedFields>
    inline bool FileReader<Data, NumFixedFields>::
    Comment(ECommentAction action)
  {
    switch (action)
    {
    case kCommentOn:
      fComment=true;
      break;
    case kCommentOff:
      fComment=false;
      break;
    case kCommentToggle:
      fComment=!fComment;
      break;
    default:
      break;
    }
    return fComment;
  }

  //--------------------------------------------------------------------------
  
  // Return the next logical record.  Always read one record ahead so
  // fMore can be set correctly.
  template<class Data, int NumFixedFields>
  inline Data FileReader<Data, NumFixedFields>::ReadData()
  {
    Data data = fNext;
    ReadNextRecord();
    return data;
  }

  //--------------------------------------------------------------------------

  // Read the next record, setting fMore appropriately.  Ignores
  // anything past last column specified by header.
  // Todo: skip comments.
  template<class Data, int NumFixedFields>
  inline void FileReader<Data, NumFixedFields>::ReadNextRecord()
  {
    // Don't bother reading if there is no more data
    if (fMore)
    {
      std::string sline;
      // Skip comments
      do
      {
	getline(fFile, sline);
      } while( fComment && fFile && sline[0] == '#' );
    
      fMore = (sline.length() > 0);
      if (fMore)
      {
	std::stringstream line(sline);
	line >> fNext;
      }
    }
  }

  //--------------------------------------------------------------------------

  // Get read-access to data that will be fetched next.
  template<class Data, int NumFixedFields>
    inline const Data& FileReader<Data, NumFixedFields>::
    ViewNextData() const
  {
    return fNext;
  }

  
} // namespace

#endif // NISAC_FILE_FILEREADER

//--------------------------------------------------------------------------
// FileReader.h
//--------------------------------------------------------------------------
