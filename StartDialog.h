#ifndef STARTDIALOG_H
#define STARTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include <opencv2/bgsegm.hpp> //for MOG, GMG
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>


enum TrackerIndices {
    NON = 0,
    BOOSTING,
    MIL,
    MEDIANFLOW,
    MOOSE,
    TLD,
    KCF,
    CSRT,
    GOTURN
};

class StartDialog : public QDialog
{
    Q_OBJECT

public:
    StartDialog(QWidget *parent = NULL);

public slots:
    void startApp();

private slots:
    void selectVideoFile();
    void selectBboxFile();

    void setVideoFileName(QString videoFileName);
    void setBboxFileName(QString bboxFileName);

    void setBoostingTrack(bool enabled);
    void setMILTrack(bool enabled);
    void setMooseTrack(bool enabled);
    void setMedianFlowTrack(bool enabled);
    void setTLDTrack(bool enabled);
    void setKCFTrack(bool enabled);
    void setCSRTTrack(bool enabled);
    void setGoturnTrack(bool enabled);

    void setSaveBbox(bool save);

private:
    void createTracker(cv::Ptr<cv::Tracker>& tracker, TrackerIndices idx);
    void writeBboxInFile(std::ofstream& file, const cv::Rect2d& bbox);
    void disableSaveBboxPart(bool disable);
    void disableVideoPart(bool disable);
    void disableState(bool disable);

private:
    QString m_videoFileName;
    QString m_bboxFileName;
    TrackerIndices m_trackerIdx;
    std::string m_trackerNames[9] = {"", "BOOSTING", "MIL", "MEDIANFLOW", "MOOSE","TLD", "KCF", "CSRT", "GOTURN"};

    //bbox part
    QLineEdit* m_bboxFileLineEdit;
    QPushButton* m_bboxFileBrowseButton;
    QLabel* m_bboxFileNameLabel;
    bool m_saveBboxInFile;
    QCheckBox* m_saveBboxCoordsCheckBox;

    //video part
    QLineEdit* m_videoFileLineEdit;
    QPushButton* m_videoBrowseButton;
    QLabel* m_videoFileNameLabel;


    QPushButton* m_startButton;

};

#endif // STARTDIALOG_H
