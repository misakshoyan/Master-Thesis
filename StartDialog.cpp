#include "StartDialog.h"
#include <QtGui>
#include <QtWidgets>

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace cv;
using namespace std;

// global variables ///////////////////////////////////////////////////////////////////////////////
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

StartDialog::StartDialog(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("Object Tracking");
    QGroupBox* objectTrackersGroupBox = new QGroupBox(tr("Object Trackers"));

    QRadioButton* boostingRadioBtn = new QRadioButton(tr("Boosting"));
    QRadioButton* milRadioBtn = new QRadioButton(tr("MIL"));
    QRadioButton* mooseRadioBtn = new QRadioButton(tr("MOOSE"));
    QRadioButton* medianFlowRadioBtn = new QRadioButton(tr("Median Flow"));
    QRadioButton* tldRadioBtn = new QRadioButton(tr("TLD"));
    QRadioButton* kcfRadioBtn = new QRadioButton(tr("KCF"));
    QRadioButton* csrtRadioBtn = new QRadioButton(tr("CSRT"));
    QRadioButton* goturnRadioBtn = new QRadioButton(tr("GOTURN"));

    csrtRadioBtn->setChecked(true);
    m_trackerIdx = TrackerIndices::CSRT;

    connect(boostingRadioBtn, SIGNAL(toggled(bool)), this, SLOT(setBoostingTrack(bool)));
    connect(milRadioBtn, SIGNAL(toggled(bool)), this, SLOT(setMILTrack(bool)));
    connect(mooseRadioBtn, SIGNAL(toggled(bool)), this, SLOT(setMooseTrack(bool)));
    connect(medianFlowRadioBtn, SIGNAL(toggled(bool)), this, SLOT(setMedianFlowTrack(bool)));
    connect(tldRadioBtn, SIGNAL(toggled(bool)), this, SLOT(setTLDTrack(bool)));
    connect(kcfRadioBtn, SIGNAL(toggled(bool)), this, SLOT(setKCFTrack(bool)));
    connect(csrtRadioBtn, SIGNAL(toggled(bool)), this, SLOT(setCSRTTrack(bool)));
    connect(goturnRadioBtn, SIGNAL(toggled(bool)), this, SLOT(setGoturnTrack(bool)));

    QVBoxLayout* trackersVLayout = new QVBoxLayout();
    trackersVLayout->addWidget(boostingRadioBtn);
    trackersVLayout->addWidget(milRadioBtn);
    trackersVLayout->addWidget(mooseRadioBtn);
    trackersVLayout->addWidget(medianFlowRadioBtn);
    trackersVLayout->addWidget(tldRadioBtn);
    trackersVLayout->addWidget(kcfRadioBtn);
    trackersVLayout->addWidget(csrtRadioBtn);
    trackersVLayout->addWidget(goturnRadioBtn);

    QGroupBox* otherSettingsGroupBox = new QGroupBox(tr("Other Settings"));
    QLabel* pauseLabel    = new QLabel(tr("-      \"p\"     key  -  pause video"));
    QLabel* continueLabel = new QLabel(tr("-  \"Enter\" key  -  continue video after pause"));
    QLabel* escLabel      = new QLabel(tr("-     \"Esc\"  key  -  exit from video processing"));
    QLabel* saveLabel     = new QLabel(tr("-     Bounding box is saved as (top left X, top left Y, width, height)"));

    m_saveBboxCoordsCheckBox = new QCheckBox(tr("Save object's Bounding Box coordinates"));
    m_saveBboxCoordsCheckBox->setChecked(true);
    connect(m_saveBboxCoordsCheckBox, SIGNAL(clicked(bool)), this, SLOT(setSaveBbox(bool)));

    QVBoxLayout* otherSettingsVBLayout = new QVBoxLayout();
    otherSettingsVBLayout->addWidget(pauseLabel);
    otherSettingsVBLayout->addWidget(continueLabel);
    otherSettingsVBLayout->addWidget(escLabel);
    otherSettingsVBLayout->addWidget(saveLabel);
    otherSettingsVBLayout->addWidget(m_saveBboxCoordsCheckBox);
    otherSettingsGroupBox->setLayout(otherSettingsVBLayout);


    m_startButton = new QPushButton("Start");
    m_startButton->setFixedSize(m_startButton->sizeHint());
    connect(m_startButton, SIGNAL(clicked(bool)), this, SLOT(startApp()));

    //video browse button
    m_videoBrowseButton = new QPushButton(tr("Browse"), this);
    connect(m_videoBrowseButton, SIGNAL(clicked(bool)), this, SLOT(selectVideoFile()));

    m_videoFileNameLabel = new QLabel(tr("Video File Name"));
    m_videoFileLineEdit = new QLineEdit;
    connect(m_videoFileLineEdit, SIGNAL(textEdited(QString)), this, SLOT(setVideoFileName(QString)));

    QHBoxLayout* videoHLayout = new QHBoxLayout;
    videoHLayout->addWidget(m_videoFileNameLabel);
    videoHLayout->addWidget(m_videoFileLineEdit);
    videoHLayout->addWidget(m_videoBrowseButton);


    //bbox file browse button
    m_bboxFileBrowseButton = new QPushButton(tr("Browse"), this);
    connect(m_bboxFileBrowseButton, SIGNAL(clicked(bool)), this, SLOT(selectBboxFile()));

    m_bboxFileNameLabel = new QLabel(tr("Bounding Box File Name"));
    m_videoFileNameLabel->setFixedSize(m_bboxFileNameLabel->sizeHint());
    m_bboxFileLineEdit = new QLineEdit;
    connect(m_bboxFileLineEdit, SIGNAL(textEdited(QString)), this, SLOT(setBboxFileName(QString)));

    QHBoxLayout* bboxHLayout = new QHBoxLayout;
    bboxHLayout->addWidget(m_bboxFileNameLabel);
    bboxHLayout->addWidget(m_bboxFileLineEdit);
    bboxHLayout->addWidget(m_bboxFileBrowseButton);

    objectTrackersGroupBox->setLayout(trackersVLayout);

    QHBoxLayout* trackersStartHBLayout = new QHBoxLayout;
    trackersStartHBLayout->addWidget(objectTrackersGroupBox);
    trackersStartHBLayout->addWidget(otherSettingsGroupBox);
    //trackersStartHBLayout->addSpacing(200);
    trackersStartHBLayout->addWidget(m_startButton);


    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(trackersStartHBLayout);
    mainLayout->addLayout(videoHLayout);
    mainLayout->addLayout(bboxHLayout);

    setLayout(mainLayout);
}

void StartDialog::createTracker(Ptr<Tracker>& tracker, TrackerIndices idx)
{
    switch (idx) {
    case TrackerIndices::BOOSTING:
        tracker = TrackerBoosting::create();
        break;
    case TrackerIndices::MIL:
        tracker = TrackerMIL::create();
        break;
    case TrackerIndices::MEDIANFLOW:
        tracker = TrackerMedianFlow::create();
        break;
    case TrackerIndices::TLD:
        tracker = TrackerTLD::create();
        break;
    case TrackerIndices::MOOSE:
        tracker = TrackerMOSSE::create();
        break;
    case TrackerIndices::KCF:
        tracker = TrackerKCF::create();
        break;
    case TrackerIndices::CSRT:
        tracker = TrackerCSRT::create();
        break;
    case TrackerIndices::GOTURN:
        tracker = TrackerGOTURN::create();
        break;
    default:
        break;
    }
}

void StartDialog::writeBboxInFile(ofstream& file, const Rect2d& bbox)
{
    if (m_saveBboxInFile) {
        stringstream sstr;
        sstr << (int)bbox.x << "," << (int)bbox.y << "," <<  (int)bbox.width << "," <<  (int)bbox.height;
        file << sstr.str() << endl;
    }
}

void StartDialog::startApp()
{
    if (m_videoFileName.isEmpty()) {
        QMessageBox::warning(this, tr("Movement Detection"), tr("Please select the video file!"),
                                        QMessageBox::Ok);
        return;
    }

    cv::VideoCapture capVideo;

    capVideo.open(m_videoFileName.toStdString());


    if (!capVideo.isOpened()) { // if unable to open video file, show error message
        QMessageBox::warning(this, tr("Object Tracking"), tr("error reading video file!"),
                                        QMessageBox::Ok);
        return;
    }

    if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {
        QMessageBox::warning(this, tr("Object Tracking"), tr("error: video file must have at least two frames!"),
                                        QMessageBox::Ok);
        return;
    }

    assert(m_trackerIdx != TrackerIndices::NON);

    ofstream ofStr;
    if (m_saveBboxInFile) {
        if (m_bboxFileName.isEmpty()) {
            QMessageBox::warning(this, tr("Movement Detection"), tr("Please specify the Bounding Box file!"),
                                 QMessageBox::Ok);
            return;
        }
        ofStr.open(m_bboxFileName.toStdString());
    }

    // Create a tracker
    Ptr<Tracker> tracker;
    createTracker(tracker, m_trackerIdx);

    // Read first frame
    Mat frame;
    Rect2d dummyBbox;
    bool ok = capVideo.read(frame);

    if (m_videoFileName == QString("/home/misak/Videos/bolt.mp4")) {
        for(int i = 0; i <= 100; i++)
            capVideo.read(frame);
    }

    // Define initial boundibg box
    //Rect2d bbox(287, 23, 86, 320);

    // Uncomment the line below to select a different bounding box
    Rect2d bbox = selectROI("Select Bounding Box" ,frame, false);
    cv::destroyWindow("Select Bounding Box");

    disableState(true);

    // Display bounding box.
    rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
    imshow("Tracking", frame);

    ok = tracker->init(frame, bbox);
    if(!ok) {
        writeBboxInFile(ofStr, dummyBbox);
    } else {
        writeBboxInFile(ofStr, bbox);
    }

    TrackerIndices idx = m_trackerIdx;
    bool track = true;

    while(track)
    {
        if (m_trackerIdx == idx) {
            track = capVideo.read(frame);
            if (track) {
                int ms = 33;
                struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
                nanosleep(&ts, NULL);
                // Start timer
                double timer = (double)getTickCount();

                // Update the tracking result
                bool ok = tracker->update(frame, bbox);

                // Calculate Frames per second (FPS)
                float fps = getTickFrequency() / ((double)getTickCount() - timer);

                if (ok) {
                    writeBboxInFile(ofStr, bbox);
                    // Tracking success : Draw the tracked object
                    rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
                } else {
                    writeBboxInFile(ofStr, dummyBbox);
                    // Tracking failure detected.
                    putText(frame, "Tracking failure detected", Point(100,80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
                }

                // Display tracker type on frame
                putText(frame, m_trackerNames[m_trackerIdx] + " Tracker", Point(100,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50),2);

                // Display FPS on frame
                putText(frame, "FPS : " + SSTR(int(fps)), Point(100,50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);

                // Display frame.
                imshow("Tracking", frame);

                // Exit if ESC pressed.
                int k = waitKey(1);
                if(k == 27)
                {
                    break;
                }

                if (k == 112) { // p key pressed
                    bool enterKeyPressed = false;
                    while (!enterKeyPressed) {
                        k = cv::waitKey(0);
                        if (k == 13) { // Enter key pressed
                            enterKeyPressed = true;
                        }
                    }

                }
            }
        } else {
            assert(m_trackerIdx != TrackerIndices::NON);
            createTracker(tracker, m_trackerIdx);
            tracker->init(frame, bbox);
            idx = m_trackerIdx;
        }
    }
    disableState(false);
    cv::destroyAllWindows();
}

void StartDialog::selectVideoFile()
{
    QString fileName =  QFileDialog::getOpenFileName(this,
                                        tr("Open Video File"), "/home/misak/Videos", tr("Video Files (*.mp4 *.avi)"));

    if (!fileName.isEmpty()) {
        setVideoFileName(fileName);
        m_videoFileLineEdit->setText(fileName);
    }
}

void StartDialog::selectBboxFile()
{
    QString fileName =  QFileDialog::getOpenFileName(this,
                                        tr("Select Bounding Box File"), "/home/misak/Videos", tr("Bounding Box Files (*)"));

    if (!fileName.isEmpty()) {
        setBboxFileName(fileName);
        m_bboxFileLineEdit->setText(fileName);
    }
}

void StartDialog::setBoostingTrack(bool enabled)
{
    if (enabled) {
        m_trackerIdx = TrackerIndices::BOOSTING;
    }
}

void StartDialog::setMILTrack(bool enabled)
{
    if (enabled) {
        m_trackerIdx = TrackerIndices::MIL;
    }
}

void StartDialog::setMooseTrack(bool enabled)
{
    if (enabled) {
        m_trackerIdx = TrackerIndices::MOOSE;
    }
}

void StartDialog::setMedianFlowTrack(bool enabled)
{
    if (enabled) {
        m_trackerIdx = TrackerIndices::MEDIANFLOW;
    }
}

void StartDialog::setTLDTrack(bool enabled)
{
    if (enabled) {
        m_trackerIdx = TrackerIndices::TLD;
    }
}

void StartDialog::setKCFTrack(bool enabled)
{
    if (enabled) {
        m_trackerIdx = TrackerIndices::KCF;
    }
}

void StartDialog::setCSRTTrack(bool enabled)
{
    if (enabled) {
        m_trackerIdx = TrackerIndices::CSRT;
    }
}

void StartDialog::setGoturnTrack(bool enabled)
{
    if (enabled) {
        m_trackerIdx = TrackerIndices::GOTURN;
    }
}

void StartDialog::setSaveBbox(bool save)
{
    m_saveBboxInFile = save;
    disableSaveBboxPart(!save);
}

void StartDialog::disableSaveBboxPart(bool disable)
{
    m_bboxFileNameLabel->setDisabled(disable);
    m_bboxFileLineEdit->setDisabled(disable);
    m_bboxFileBrowseButton->setDisabled(disable);
}

void StartDialog::disableVideoPart(bool disable)
{
    m_videoFileNameLabel->setDisabled(disable);
    m_videoFileLineEdit->setDisabled(disable);
    m_videoBrowseButton->setDisabled(disable);
}

void StartDialog::disableState(bool disable)
{
    m_saveBboxCoordsCheckBox->setDisabled(disable);
    if (m_saveBboxInFile) {
        disableSaveBboxPart(disable);
    }
    disableVideoPart(disable);
    m_startButton->setDisabled(disable);
}

void StartDialog::setVideoFileName(QString videoFileName)
{
    m_videoFileName = videoFileName;
}

void StartDialog::setBboxFileName(QString bboxFileName)
{
    m_bboxFileName = bboxFileName;
}
