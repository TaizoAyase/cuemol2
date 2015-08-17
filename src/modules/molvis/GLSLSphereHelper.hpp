// -*-Mode: C++;-*-
//
//  GLSL sphere rendering helper class
//

#ifndef GLSL_SPHERE_HELPER_HPP_INCLUDED
#define GLSL_SPHERE_HELPER_HPP_INCLUDED

namespace molvis {

  class GLSLSphereHelper
  {
  private:
    
    struct SphElem {
      qfloat32 cenx, ceny, cenz;
      qfloat32 dspx, dspy;
      qfloat32 rad;
      qbyte r, g, b, a;
    };
    
    typedef gfx::DrawAttrElems<quint16, SphElem> SphElemAry;

    GLuint m_nVertexLoc;
    GLuint m_nImposLoc;
    GLuint m_nRadLoc;
    GLuint m_nColLoc;

    sysdep::OglProgramObject *m_pPO;

    SphElemAry *m_pDrawElem;

    qfloat32 dsps[4][2];

  public:
    GLSLSphereHelper()
         : m_pPO(NULL), m_pDrawElem(NULL)
    {
      
      dsps[0][0] = -1.0f; dsps[0][1] = -1.0f;
      dsps[1][0] =  1.0f, dsps[1][1] = -1.0f;
      dsps[2][0] = -1.0f, dsps[2][1] =  1.0f;
      dsps[3][0] =  1.0f, dsps[3][1] =  1.0f;
    }

    ~GLSLSphereHelper()
    {
      invalidate();
    }

    bool initShader(qsys::Renderer *pRend)
    {
      MB_ASSERT(m_pPO == NULL);
      sysdep::ShaderSetupHelper<qsys::Renderer> ssh(pRend);
      
      if (!ssh.checkEnvVS()) {
        MB_DPRINTLN("GLShader not supported");
        return false;
      }
      
      if (m_pPO==NULL)
        m_pPO = ssh.createProgObj("gpu_sphere",
                                  "%%CONFDIR%%/data/shaders/sphere_vertex.glsl",
                                  "%%CONFDIR%%/data/shaders/sphere_frag.glsl");
      
      if (m_pPO==NULL) {
        LOG_DPRINTLN("GPUSphere> ERROR: cannot create progobj.");
        return false;
      }
      
      // setup attributes
      m_nVertexLoc = m_pPO->getAttribLocation("a_vertex");
      m_nImposLoc = m_pPO->getAttribLocation("a_impos");
      m_nRadLoc = m_pPO->getAttribLocation("a_radius");
      m_nColLoc = m_pPO->getAttribLocation("a_color");

      return true;
    }

    void alloc(int nsphs)
    {
      SphElemAry *pdata = MB_NEW SphElemAry();
      m_pDrawElem = pdata;
      SphElemAry &sphdata = *pdata;
      sphdata.setAttrSize(4);
      sphdata.setAttrInfo(0, m_nVertexLoc, 3, qlib::type_consts::QTC_FLOAT32,  offsetof(SphElem, cenx));
      sphdata.setAttrInfo(1, m_nImposLoc, 2, qlib::type_consts::QTC_FLOAT32, offsetof(SphElem, dspx));
      sphdata.setAttrInfo(2, m_nRadLoc, 1, qlib::type_consts::QTC_FLOAT32, offsetof(SphElem, rad));
      sphdata.setAttrInfo(3, m_nColLoc, 4, qlib::type_consts::QTC_UINT8, offsetof(SphElem, r));
      
      sphdata.alloc(nsphs*4);
      sphdata.allocInd(nsphs*6);
      sphdata.setDrawMode(gfx::AbstDrawElem::DRAW_TRIANGLES);
    }

    void setData(int ind, const Vector4D &pos, double rad, ColorPtr pc)
    {
      //qfloat32 dsps[4][2] = ;
      
      int i = ind * 4;
      int ifc = ind * 6;

      SphElemAry &sphdata = *m_pDrawElem;
      SphElem data;

      data.cenx = (qfloat32) pos.x();
      data.ceny = (qfloat32) pos.y();
      data.cenz = (qfloat32) pos.z();
      data.rad = (qfloat32) rad;
      data.r = (qbyte) pc->r();
      data.g = (qbyte) pc->g();
      data.b = (qbyte) pc->b();
      data.a = (qbyte) pc->a();

      sphdata.atind(ifc) = i + 0; ++ifc;
      sphdata.atind(ifc) = i + 1; ++ifc;
      sphdata.atind(ifc) = i + 2; ++ifc;
      sphdata.atind(ifc) = i + 2; ++ifc;
      sphdata.atind(ifc) = i + 1; ++ifc;
      sphdata.atind(ifc) = i + 3; ++ifc;

      for (int j=0; j<4; ++j) {
        sphdata.at(i) = data;
        sphdata.at(i).dspx = dsps[j][0];
        sphdata.at(i).dspy = dsps[j][1];
	++i;
      }
    }

    gfx::AbstDrawElem *getDrawElem() const
    {
      return m_pDrawElem;
    }

    void draw(DisplayContext *pdc)
    {
      m_pPO->enable();
      pdc->drawElem(*m_pDrawElem);
      m_pPO->disable();
    }

    void invalidate()
    {
      if (m_pDrawElem!=NULL) {
        delete m_pDrawElem;
        m_pDrawElem = NULL;
      }
    }

  };

}

#endif
