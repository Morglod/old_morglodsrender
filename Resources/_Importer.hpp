#pragma once

#ifndef _MR_IMPORTER_H_
#define _MR_IMPORTER_H_

#include <string>
#include <fstream>

namespace MR {

class Importer;
class ImporterStage;
class ImporterStage_Anims;
class ImporterStage_Cams;
class ImporterStage_Lights;
class ImporterStage_Materials;
class ImporterStage_Meshes;
class ImporterStage_Textures;

class ImporterStage {
    friend class Importer;
public:
    enum {
        STAGE_ANIM = 0,
        STAGE_CAM = 1,
        STAGE_LIGHT,
        STAGE_MATERIAL,
        STAGE_MESH,
        STAGE_TEXTURE,
        STAGES_NUM
    };

    inline char GetStage() { return _stage; }
    inline ImporterStage* GetNext() { return _next; }
    inline ImporterStage* GetPrev() { return _prev; }
    inline Importer* GetImporter() { return _importer; }

    virtual ImporterStage* GoNext() = 0; //_GoToEnd() and return _next
    virtual bool Import() = 0; //Import stage fully

    ImporterStage(const char& stage, ImporterStage* next, ImporterStage* prev, Importer* imp);
    virtual ~ImporterStage();
protected:
    virtual void _GoToEnd() = 0; //read all stage, to the beginning of next

    char _stage;
    ImporterStage* _next;
    ImporterStage* _prev;
    Importer* _importer;

    bool _imported; //should be true, after Import() or _GoToEnd()
};

class ImporterStage_Anims : public ImporterStage {
public:
    inline ImporterStage* GoNext() override {
        _GoToEnd();
        return _next;
    }

    bool Import() override;

    ImporterStage_Anims(ImporterStage* next, ImporterStage* prev, Importer* imp);
    virtual ~ImporterStage_Anims();

protected:
    void _GoToEnd() override;
};

class ImporterStage_Cams : public ImporterStage {
public:
    inline ImporterStage* GoNext() override {
        _GoToEnd();
        return _next;
    }

    bool Import() override;

    ImporterStage_Cams(ImporterStage* next, ImporterStage* prev, Importer* imp);
    virtual ~ImporterStage_Cams();

protected:
    void _GoToEnd() override;
};

class ImporterStage_Lights : public ImporterStage {
public:
    inline ImporterStage* GoNext() override {
        _GoToEnd();
        return _next;
    }

    bool Import() override;

    ImporterStage_Lights(ImporterStage* next, ImporterStage* prev, Importer* imp);
    virtual ~ImporterStage_Lights();

protected:
    void _GoToEnd() override;
};

class ImporterStage_Materials : public ImporterStage {
public:
    inline ImporterStage* GoNext() override {
        _GoToEnd();
        return _next;
    }

    bool Import() override;

    ImporterStage_Materials(ImporterStage* next, ImporterStage* prev, Importer* imp);
    virtual ~ImporterStage_Materials();

protected:
    void _GoToEnd() override;
};

class ImporterStage_Meshes : public ImporterStage {
public:
    inline ImporterStage* GoNext() override {
        _GoToEnd();
        return _next;
    }

    bool Import() override;

    ImporterStage_Meshes(ImporterStage* next, ImporterStage* prev, Importer* imp);
    virtual ~ImporterStage_Meshes();

protected:
    void _GoToEnd() override;
};

class ImporterStage_Textures : public ImporterStage {
public:
    inline ImporterStage* GoNext() override {
        _GoToEnd();
        return _next;
    }

    bool Import() override;

    ImporterStage_Textures(ImporterStage* next, ImporterStage* prev, Importer* imp);
    virtual ~ImporterStage_Textures();

protected:
    void _GoToEnd() override;
};

class Importer {
    friend class ImporterStage;
public:
    inline ImporterStage* GetCurrent() { return _current; }

    static Importer* Open(const std::string& file);

    ~Importer();
protected:
    Importer();
    bool _OpenFile(const std::string& file);

    std::ifstream _file;
    ImporterStage* _stages[ImporterStage::STAGES_NUM];
    ImporterStage* _current;

    int _numAnims;
    int _numCams;
    int _numLights;
    int _numMaterials;
    int _numMeshes;
    int _numTextures;

    bool _log;
};

}

#endif
