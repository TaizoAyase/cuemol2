//
//
//

#ifndef THEBES_TEXT_RENDER_HPP_INCLUDED__
#define THEBES_TEXT_RENDER_HPP_INCLUDED__

#include "xpcom.hpp"

#include <nsIDocShell.h>

#include <gfxContext.h>
#include <gfxImageSurface.h>
//#include <gfxPlatform.h>

//#define nsString_h___
//#include <gfxFont.h>
//#include <gfxTextRunCache.h>

//////////

#include <qlib/LString.hpp>
#include <gfx/TextRenderManager.hpp>
#include <gfx/PixelBuffer.hpp>

#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>

using qlib::LString;

//////////

class ThebesTextRender : public gfx::TextRenderImpl
{
private:

  gfxContext *mThebesContext;
  gfxASurface *mThebesSurface;
//gfxFontGroup *mFontGroup;
  // gfxFontStyle *mFontStyle;

  nsString mTextStyle;

  std::vector<QUE_BYTE> m_bytes;

  typedef boost::const_multi_array_ref<QUE_BYTE, 3> ConstArrayRef;
  typedef boost::multi_array_ref<QUE_BYTE, 3> ArrayRef;

  QUE_BYTE bit_reverse(QUE_BYTE bData)
  {
    QUE_BYTE lookup[] =
    { 0, 8,  4, 12,  
      2, 10, 6, 14 ,  
      1, 9,  5, 13, 
      3, 11, 7, 15 }; 
    QUE_BYTE ret_val = (QUE_BYTE)(((lookup[(bData & 0x0F)]) << 4) + lookup[((bData & 0xF0) >> 4)]); 
    return ret_val; 
  }

  /// Convert Thebes image surface data to qlib PixelBuffer object
  void convertToPixelBuffer(gfx::PixelBuffer &bytes, int w, int h);

  int m_n;

  double m_dFontSize;
  LString m_strFontName;
  PRUint8 m_nFontStyle;
  PRUint16 m_nFontWeight;
  bool m_bUseAA;

  void setupGlobalFontStyle();
  void makeFontGroup();

  typedef std::multiset<nsIDocShell *> DocShellTab;
  DocShellTab m_docset;

public:

  ThebesTextRender();

  virtual ~ThebesTextRender();

  virtual bool renderText(const qlib::LString &str, gfx::PixelBuffer &buf);

  virtual void setMouseCursor(int ncursor) {}

  //////////

  bool setupFont(double fontsize, const LString &fontname,
                 const LString &font_style,
                 const LString &font_wgt);

  bool makeSurface(int w, int h);

//bool measureText(const LString &str, gfxTextRun::Metrics &rval);


  void addDocShell(nsIDocShell *docShell)
  {
    m_docset.insert(docShell);
  }

  bool removeDocShell(nsIDocShell *docShell)
  {
    DocShellTab::iterator iter = m_docset.find(docShell);
    if (iter==m_docset.end())
      return false;
    m_docset.erase(iter);
    return true;
  }

  nsIDocShell *getDocShell()
  {
    DocShellTab::const_iterator iter = m_docset.begin();
    if (iter==m_docset.end())
      return NULL;
    return *iter;
  }
  //virtual bool renderTextImpl(gfxPoint xxx, const qlib::LString &str, gfx::PixelBuffer &buf);

}; //class ThebesTextRender

#endif

