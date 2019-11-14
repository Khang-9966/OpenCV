#ifndef OPENILIMAGE_HPP
#define OPENILIMAGE_HPP

#include "ITImage.h"


class ITIMAGE_API OpenILImage {
	
public:

	struct ITIMAGE_API idHandler {

		unsigned int id;
		idHandler(); 
		~idHandler();
		operator unsigned int() const { return id; }

	} imageId;


	OpenILImage(const ITImage&);
	OpenILImage(const OpenILImage&);
	OpenILImage(unsigned int);
	OpenILImage(std::string);

	OpenILImage& operator=(const OpenILImage&);

	operator ITImage() const;
	void bind() const;
	void write(std::string) const;

};


#endif