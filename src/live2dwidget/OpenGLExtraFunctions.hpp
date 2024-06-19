#ifndef OPENGLEXTRAFUNCTIONS_H
#define OPENGLEXTRAFUNCTIONS_H

#include <QOpenGLExtraFunctions>

class OpenGLExtraFunctions {
public:
    inline static QOpenGLFunctions* GetInstance(){
        return instance;
    }
    inline static void ReleaseInstance(){
        if(instance!=nullptr)
            delete instance;
        instance=nullptr;
    }
    inline static void InitInstance(QOpenGLContext * context)
    {
        if(instance==nullptr)
        {
            instance=new QOpenGLFunctions(context);
            instance->initializeOpenGLFunctions();
        }
    }
private:
    inline OpenGLExtraFunctions(){}
    inline ~OpenGLExtraFunctions(){}
    inline static QOpenGLFunctions* instance=nullptr;
};

#endif // OPENGLEXTRAFUNCTIONS_H
