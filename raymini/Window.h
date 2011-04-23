#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QCheckBox>
#include <QGroupBox>
#include <QComboBox>
#include <QSlider>
#include <QLCDNumber>
#include <QSpinBox>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QProgressBar>

#include <vector>
#include <string>

#include "QTUtils.h"
#include "GLViewer.h"

class Window : public QMainWindow {
    Q_OBJECT
public:
        enum OptionFlag 
        {
            NONE = 0,
            ENABLE_AO = 0x2,
            ENABLE_AA = 0x4         
        };

        Window();
        virtual ~Window();

        static void showStatusMessage (const QString & msg);
        inline unsigned int getFlags() { return flags; }         

public slots :
        void displayTime (QTime);
        void enableAO (bool);
        void updateAO (int value);
        void enableAA (bool);
        void updateAA (int value);
        void renderRayImage ();
        void setBGColor ();
        void exportGLImage ();
        void exportRayImage ();
        void about ();

signals:
        void updateTime(QString);

private:

        void initControlWidget ();

        QActionGroup * actionGroup;
        QGroupBox * controlWidget;
        QString currentDirectory;

        GLViewer * viewer;
        QLabel * imageLabel;
        QImage rayImage;
        QLabel* timeDisplayLabel;
        QProgressBar * renderBar;
        unsigned int flags;
};

#endif // WINDOW_H


// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
