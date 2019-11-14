#ifndef sliceset_hpp
#define sliceset_hpp

// helps with dealing with sets of images
// include "genfilename"

#pragma warning(disable: 4786)
#pragma warning(disable: 4251)


#include <vector>
#include <string>
#ifndef SWIG
#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#endif
#include "ImageTools.h"
#include "ITImage.H"

class ITImage;


/// given a template of a name, create file names from indices
class ITIMAGE_API GenFileName {
	
	
public:
#ifndef SWIG
	mutable boost::format nameTemplate;
#endif
	std::string nameDirectory;
	std::string nameFormat;
	
	int first;
	std::string fullname;
	explicit GenFileName(std::string _dir, std::string _fmt, int _first = 0)
		: nameDirectory(_dir), nameFormat(_fmt), nameTemplate(_fmt), first(_first), fullname(_dir + "/" + _fmt)
	{
		if (_fmt.find_first_of('%') == std::string::npos) {
			IT_WARN("attempt to create GenFileName with no format string in it!");
		}
	}
	
	/// create a "Null" generator (its the empty set of files)
	GenFileName()
		: nameDirectory(""), nameFormat("%d"), nameTemplate("%d"), first(0)
	{}
	
#ifndef SWIG
	/// returns a filename no matter what
	std::string operator[](int i) const;
#endif

	/// check to see if the file name that would be returned exists
	bool exists(int) const;

	//sets the directory and format externally after a genfilename has been created
	void setFormatTemplate(const char* dir, const char* name);


};

ITIMAGE_API bool fileExists(const std::string&);

typedef boost::shared_ptr<ITImage> ImagePtr;



/// a set of images with consecutive file names
/** implements lazy evaluation such that images are not loaded until they 
 ** are needed **/
class ITIMAGE_API ImageDir {

protected:

	int	count;

	GenFileName nameFormatter;

	mutable std::vector<ImagePtr> cache;

	void countFiles()
	{	
		for(count = 0; nameFormatter.exists(count); ++count); 
		cache.resize(count);
	}


public:
	ImageDir() 
		: count(0) 
	{}

	explicit ImageDir(const std::string& dir, const std::string& format, int first=0)
		:	nameFormatter(GenFileName(dir,format,first))
	{	countFiles();	}

	void setFormat(std::string& dir, std::string fmt, int first)
	{	
		nameFormatter = GenFileName(dir, fmt, first);
		countFiles();	
	}

	//virtual int size() const 
	virtual int size() const
	{	return count; }

	virtual ITImage& get(int) const;
};

#endif
