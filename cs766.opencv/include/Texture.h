#pragma once

#include "glut.h"
#include "imdllexp.h"
class ITImage;
class HierWarp;
#include "RefPtr.h"
#include "imdllexp.h"
#include <utility>

#pragma warning(push)
#pragma warning(disable:4231)
IM_STL_DLL template struct IM_DLL std::pair<double, double>;
#pragma warning(pop)

/**
 * Texture base class
 */
class IM_DLL Texture {
public:
	/**
	 * Construct a base texture
	 * @param image Image to make the texture from
	 * @param num Number of texture units that will be needed
	 */
	Texture(const ITImage& image, int num=1);
	virtual ~Texture();

private: // disallow copy/assignment
	Texture(const Texture& rhs);
	Texture& operator=(const Texture& rhs);

protected:
	
	virtual GLenum channelsToGLFormat(int num) const;

public:
	virtual GLenum getTarget() const=0;
	virtual void bind(int i=0) const;
	virtual void unbind() const;
	virtual void draw() const=0;
    virtual void copyRegionToTexture(const ITImage& img, int destX, int destY)=0;

    /**
     * Draw a triangle in GL.
     * @param pts double[6] of the three pts of the triangle
     * @param texPts double[6] of the texture points of the triangle
     */
	virtual void glTri2dv(const double* const pts, const double* const texPts) const=0;
    
    /**
     * Convert from unit coords to texture coords
     */
    virtual std::pair<double, double> toTex(const std::pair<double, double>& input);
    virtual std::pair<double, double> getSize();
protected:
	int m_num;
	GLuint* m_id;
public:
	std::pair<double, double> m_size;
    std::pair<double, double> m_cent;
    std::pair<double, double> m_texOffset;
};

class IM_DLL TextureRect : public Texture {
public:
	TextureRect(const ITImage& image);
	virtual ~TextureRect();

private: // disallow copy/assignment
	TextureRect(const TextureRect& rhs);
	TextureRect& operator=(const TextureRect& rhs);

public:
	virtual GLenum getTarget() const;
	virtual void draw() const;
    /**
     * Draw a triangle in GL.
     * @param pts double[6] of the three pts of the triangle
     * @param texPts double[6] of the texture points of the triangle
     */
	virtual void glTri2dv(const double* const pts, const double* const texPts) const;
    virtual void copyRegionToTexture(const ITImage& img, int destX, int destY);
};

class IM_DLL TiledTextureRect : public Texture {
public:
	TiledTextureRect(const ITImage& image, int maxTileWidth, int maxTileHeight);
	virtual ~TiledTextureRect();

private: // disallow copy/assignment
	TiledTextureRect(const TiledTextureRect& rhs);
	TiledTextureRect& operator=(const TiledTextureRect& rhs);

public:
	virtual GLenum getTarget() const;
	virtual void draw() const;
    /**
     * Draw a triangle in GL.
     * @param pts double[6] of the three pts of the triangle
     * @param texPts double[6] of the texture points of the triangle
     */
	virtual void glTri2dv(const double* const pts, const double* const texPts) const;
    virtual void copyRegionToTexture(const ITImage& img, int destX, int destY);

protected:
	int m_numWide,
        m_numHigh,
        m_origWidth,
        m_origHeight,
        m_tileWidth,
        m_tileHeight;
    TextureRect** m_rects;

public:
    static const int TILE_SIZE;
};

#pragma warning(push)
#pragma warning(disable:4231)
IM_STL_DLL template class IM_DLL RefPtr<Texture>;
typedef RefPtr<Texture> TexturePtr;
#pragma warning(pop)

