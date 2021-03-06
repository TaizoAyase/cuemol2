// -*-Mode: C++;-*-
//
// Molecular morphing animation object class
//

#ifndef ANIM_MORPH_MOL_HPP_INCLUDED
#define ANIM_MORPH_MOL_HPP_INCLUDED

#include "anim.hpp"
#include <qlib/Array.hpp>

#include <modules/molstr/MolCoord.hpp>

namespace anim {

  using molstr::MolCoordPtr;

  typedef qlib::Array<float> PosArray;

  class FrameData : public qlib::LDataSrcContainer
  {
  public:
    FrameData() {}
    
    ~FrameData() {}
    
    LString m_name;
    LString m_src;
    LString m_altsrc;
    LString m_srctype;
    
    PosArray m_crds;
    MolCoordPtr m_pMol;
    
  public:
    // Data source container interface implementation
    virtual bool isDataSrcWritable() const;
    virtual LString getDataChunkReaderName(int nQdfVer) const;
    virtual void writeDataChunkTo(qlib::LDom2OutStream &oos) const;
    virtual void readFromStream(qlib::InStream &ins);
    virtual void setDataChunkName(const LString &name, qlib::LDom2Node *pNode, int nQdfVer);

    virtual void updateSrcPath(const LString &srcpath);
  };

  class ANIM_API MorphMol : public molstr::MolCoord
  {
    MC_SCRIPTABLE;

  private:
    typedef molstr::MolCoord super_t;

    /////////////////////////////////////////////////////
    // specific data

    /// number of atoms in each frame
    int m_nAtoms;
    
    std::vector<int> m_id2aid;

    typedef std::deque<FrameData *> FrameArray;

    FrameArray m_frames;

  public:
    
    /////////////////////////////////////////////////////
    // construction/destruction
    
    MorphMol();
    
    virtual ~MorphMol();
    
    /// Detached from ObjReader (i.e. end of loading)
    // virtual void readerDetached();

    /////////////////////////////////////////////////////
    // specific operations
    
    /// Insert/Append new coordinates frame
    void insertBefore(MolCoordPtr pmol, int index);
    
    /// Remove coordinates frame
    void removeFrame(int index);

    LString getFrameInfoJSON() const;

    void appendThisFrame();

    /////

    void update(double dframe);
    
    int getFrameSize() const {
      return m_frames.size();
    }
    
  private:
    /// current frame value
    double m_dframe;

  public:
    void setFrame(double dframe) {
      m_dframe = dframe;
      update(dframe);
    }

    double getFrame() const {
      return m_dframe;
    }

  private: 
    /// Scale frame value
    ///  (i.e. scale 0~nframe-1 vs noscale 0~1)
    bool m_bScaleDframe;

  public:
    void setScaleFrame(bool b) {
      m_bScaleDframe = b;
      update(m_dframe);
    }

    bool isScaleFrame() const {
      return m_bScaleDframe;
    }

  public:
    ////////////////////////////////////////////////////
    // Serialization/Deserialization

    virtual void writeTo2(qlib::LDom2Node *pNode) const;
    virtual void readFrom2(qlib::LDom2Node *pNode);

    virtual void readFromStream(qlib::InStream &ins);

    virtual void forceEmbed();

    virtual void writeDataChunkTo(qlib::LDom2OutStream &oos) const;

  private:
    /// Create from mol
    //void createFromMol(molstr::MolCoordPtr pmol);
    void setupData();
    
    //MolCoordPtr readNewMol(const LString &src,
    //const LString &altsrc,
    //const LString &srctype);

  };

}

#endif

