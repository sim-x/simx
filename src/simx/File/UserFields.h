//--------------------------------------------------------------------------
// File:    UserFields.h
// Module:  File
// Author:  Keith Bisset
// Created: July  1 2002
// Description: Read the optional user fields.  Keep track of the header
// in order to identify the field names.
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_FILE_USERFIELDS
#define NISAC_FILE_USERFIELDS
#include "simx/File/Header.h"
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <vector>
#include <sstream>


namespace File {
/// \class UserFields UserFields.h "File/UserFields.h"
///
/// \brief Read the optional user fields.
///
/// Keep track of the header in order to identify the field names.
template<typename DATA>
class UserFields
{
public:
	typedef UserFields<DATA> BaseClass;
	/// Constructor
	UserFields();

	/// Constructor
	UserFields(const UserFields& );

	/// Assignment
	UserFields& operator=(const UserFields&);

	/// Set a pointer to the header object.  There is only one object
	/// per file.
	void SetHeader(HeaderPtr header);

	/// look up data by index (0 indexed from first column)
	DATA GetUserField(int index) const;

	/// look up data by field name
	DATA GetUserField(const std::string& field) const;

	/// get the index for a field.  Returns -1 if field not found
	int GetUserFieldIndex(std::string field) const;

	/// Get the field at a particular position
	const std::string& GetUserFieldName(int index) const;

  /// Return the total number of fields
  int NumberFields() const;

  /// Return the number of fixed fields
  int NumberFixedFields() const;

  /// Return the number of user fields
  int NumberUserFields() const;

	/// Get the notes field
	const std::string& Notes() const;

	/// Get the notes field
	std::string Notes();

	/// Set the notes field
	void SetNotes(const std::string& notes);

	/// Add text to the notes field.
	void AddToNotes(const std::string& notes);

	/// Read the user fields specified by the header from the stream
	void Read(std::istream& is);

	/// Print the user fields to the stream
	void Print(std::ostream& os) const;

	/// Add one additional field - returns the index of that field, -1 if field found
	int AddField(const std::string& field);

	/// Given a name, Mark a field as ignored, so that we don't print it out
	void IgnoreField(const std::string& field);

	/// Given an index, mark a field as ignored
	void IgnoreField(int idx);

	/// Set data for a field given an index.  Could be any UserField.
	bool SetData(int idx, DATA value);

	/// Check if a field is ignored
	bool IsIgnored(const std::string& field);

	/// Returns the entire header as a string - WILL NOT REMOVE IGNORED FIELDS
	const std::string& GetHeader() const;

	/// Prints out the header - will remove ignored fields
	void PrintHeader(std::ostream& os) const;

  ///returns a pointer to header object
  HeaderPtr GetHeaderPtr() const;

protected:
private:
	inline int UserNumtoFieldNum(int idx) const;
	inline int FieldNumtoUserNum(int idx) const;
	HeaderPtr fHeader;
	std::vector<DATA> fData;
	std::string fNotes;
};

template<typename DATA>
inline void UserFields<DATA>::SetHeader(HeaderPtr header)
{
  fHeader = header;
}

template<typename DATA>
inline DATA UserFields<DATA>::GetUserField(int index) const
{
  // The user fields are index from 0, index refers to the absolute
  // field number
  if (static_cast<unsigned>(index) < fData.size())
    return fData[index];
  else
    // may be better to throw an exception here.
    // For now, return a default constructed DATA object
    return DATA();
}


template<typename DATA>
inline int UserFields<DATA>::GetUserFieldIndex(std::string field) const
{
  return FieldNumtoUserNum(fHeader->GetIndex(field));
}

template<typename DATA>
inline DATA UserFields<DATA>::GetUserField(const std::string& field) const
{
  int idx = GetUserFieldIndex(field);
  return GetUserField(idx);

}

template<typename DATA>
inline const std::string& UserFields<DATA>::GetUserFieldName(int index) const
{
  return fHeader->GetFieldName(UserNumtoFieldNum(index));
}

template<typename DATA>
inline int UserFields<DATA>::NumberFields() const
{
  return fHeader->NumberFields();
}

template<typename DATA>
inline int UserFields<DATA>::NumberFixedFields() const
{
  return fHeader->NumberFixedFields();
}

template<typename DATA>
inline int UserFields<DATA>::NumberUserFields() const
{
  return fHeader->NumberUserFields();
}

template<typename DATA>
inline const std::string& UserFields<DATA>::Notes() const
{
  return fNotes;
}

template<typename DATA>
inline std::string UserFields<DATA>::Notes()
{
  return fNotes;
}

template<typename DATA>
inline void UserFields<DATA>::SetNotes(const std::string& notes)
{
  fNotes = notes;
}

template<typename DATA>
inline void UserFields<DATA>::AddToNotes(const std::string& notes)
{
  fNotes += notes;
}

template<typename DATA>
inline int UserFields<DATA>::UserNumtoFieldNum(int idx) const
{
  return idx + fHeader->NumberFixedFields();
}

template<typename DATA>
inline int UserFields<DATA>::FieldNumtoUserNum(int idx) const
{
  return idx - fHeader->NumberFixedFields();
}

/// Userfields extraction operator
template<typename DATA>
inline std::istream& operator>>(std::istream& is, UserFields<DATA>& u)
{
  u.Read(is);
  return is;
}

/// Userfields insertion operator
template<typename DATA>
inline std::ostream& operator<<(std::ostream& os, const UserFields<DATA>& u)
{
  u.Print(os);
  return os;
}

template<typename DATA>
UserFields<DATA>::UserFields()
//  : fHeader(0)
{
}

template<typename DATA>
UserFields<DATA>::UserFields(const UserFields<DATA>& rhs)
{
  if (&rhs == this)
    return;
  fHeader = rhs.fHeader;
  fNotes = rhs.fNotes;
  for (unsigned int i=0; i < rhs.fData.size(); ++i)
    fData.push_back(rhs.fData[i]);
}

template<typename DATA>
UserFields<DATA>& UserFields<DATA>::operator=(const UserFields<DATA>& rhs)
{
  if (&rhs == this)
    return *this;
  fHeader = rhs.fHeader;
  fNotes = rhs.fNotes;
  // Clear fData
  std::vector<DATA> empty;
  fData.swap(empty);

  for (unsigned int i=0; i < rhs.fData.size(); ++i)
  {
    fData.push_back(rhs.fData[i]);
  }
  return *this;
}

template<typename DATA>
void UserFields<DATA>::Read(std::istream& is)
{
  // Remove existing values
  if (fData.size() != 0)
  {
    fData.clear();
  }

  if (!fHeader)
  {
    char buf[256];
    is.getline(buf, 256);
    std::stringstream str;
    str << buf;
    while (str)
    {
      DATA s;
      str >> s;
      if (str)
        fData.push_back(s);
    }
  }
  else
  {
    for (int i=0; i < (fHeader->NumberUserFields() - fHeader->GetNumAddedUserFields()); ++i)
    {
      DATA value;
      is >> value;
      fData.push_back(value);
    }
    char buf[256];
    is.getline(buf, 256);
    char* start = buf;
    while (*start == ' ' || *start == '\t')
      start++;
    fNotes = start;
  }
}

template<typename DATA>
void UserFields<DATA>::Print(std::ostream& os) const
{
  if (!fHeader)
  {
    return;
  }
  if (fHeader->NumberUserFields() != static_cast<int>(fData.size()))
    std::cout << "\nMismatch " << fHeader->NumberUserFields() << " " << fData.size() << std::endl;
  for (unsigned int i=0; i < fData.size(); i++)
  {
    if(!(fHeader->IsIgnoredIndex(i)))   //only print out fields that have not been ignored
    {
      os << fData[i] << " ";
    }
  }
  os << fNotes;
}

template<typename DATA>
int UserFields<DATA>::AddField(const std::string& field)
{
  return fHeader->AddHeaderField(field);
}

template<typename DATA>
void UserFields<DATA>::IgnoreField(const std::string& field)
{
  fHeader->IgnoreHeaderField(field);
}

template<typename DATA>
void UserFields<DATA>::IgnoreField(int idx)
{
  fHeader->IgnoreHeaderField(GetUserFieldName(idx));
}

template<typename DATA>
bool UserFields<DATA>::SetData(int idx, DATA value)
{
  //get a new index since fData does not include fixed fields
  int adjustedIndex = idx - fHeader->NumberFixedFields();

  if((idx+1) <= NumberFields())
  {
    //this check is in case we are adding for the first time, then
    //fData would not be big enough.  +1 since vectors are zero based.
    if(static_cast<int>(fData.size()) < adjustedIndex + 1)
    {
      fData.resize(adjustedIndex + 1);
    }

    //insert value into fData
    fData[adjustedIndex] = value;
    return true;
  }
  else
  {
    //error: the field doesn't 'exist'
    return false;
  }
}

template<typename DATA>
bool UserFields<DATA>::IsIgnored(const std::string& field)
{
  return fHeader->IsIgnored(field);
}

template<typename DATA>
const std::string& UserFields<DATA>::GetHeader() const
{
  return fHeader->GetHeader();
}

template<typename DATA>
void UserFields<DATA>::PrintHeader(std::ostream& os) const
{
  fHeader->Print(os);
  os << fNotes;

}

template<typename DATA>
HeaderPtr UserFields<DATA>::GetHeaderPtr() const
{
  return fHeader;
}
} // namespace
#endif // NISAC_FILE_USERFIELD
