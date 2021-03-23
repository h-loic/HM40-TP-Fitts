#include "FittsView.h"
#include "graphicwidget.h"
#include "../Controller/fittscontroller.h"
#include "../Model/fittsmodel.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>

#include <QStackedLayout>
#include <QGraphicsView>
#include <QChart>
#include <QPolarChart>
#include <QLineSeries>

QT_CHARTS_USE_NAMESPACE

FittsView::FittsView(FittsModel *fittsModel) : QMainWindow() {
    this->fittsModel = fittsModel;

    this->initWindows();
    this->fittsController = new FittsController(this, this->fittsModel);

    // Btn clicked
    connect(leaveBtn,SIGNAL(clicked()),fittsController,SLOT(quit()));
    connect(startBtn,SIGNAL(clicked()),fittsController,SLOT(startSimulation()));

    connect(backBtn,SIGNAL(clicked()),fittsController,SLOT(backToSettings()));
    connect(resultBtn,SIGNAL(clicked()),fittsController,SLOT(resultClicked()));

    connect(resultLeaveBtn,SIGNAL(clicked()),fittsController,SLOT(quit()));
    connect(restartBtn,SIGNAL(clicked()),fittsController,SLOT(backToSettings()));

    connect(graphicView, SIGNAL(mouseClicked(int,int)), fittsController, SLOT(cibleClicked(int,int)));

    // SpinBox values update
    connect(aValue,SIGNAL(valueChanged(double)),fittsController,SLOT(aValueChanged(double)));
    connect(bValue,SIGNAL(valueChanged(double)),fittsController,SLOT(bValueChanged(double)));
    connect(nbCible,SIGNAL(valueChanged(int)),fittsController,SLOT(nbCibleChanged(int)));
    connect(minSize,SIGNAL(valueChanged(int)),fittsController,SLOT(minSizeChanged(int)));
    connect(maxSize,SIGNAL(valueChanged(int)),fittsController,SLOT(maxSizeChanged(int)));
}

FittsView::~FittsView() {}

void FittsView::initWindows() {

    QWidget *mainWidget = new QWidget;
    mainWidget->setStyleSheet("background-color:#F6F6F6;");
    this->setCentralWidget(mainWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);

    mainStack = new QStackedLayout;
    mainLayout->addLayout(mainStack);

    QWidget *settingsWidget = new QWidget;
    mainStack->addWidget(settingsWidget);

    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsWidget);


    QLabel *label;

    label = new QLabel("Programme pour vérifier la loi Fitts");
    label->setAlignment(Qt::AlignCenter);
    label->setFont(QFont("Arial",20));
    settingsLayout->addWidget(label);


    QGroupBox *rappelBox = new QGroupBox("Rappel");
    rappelBox->setStyleSheet("QGroupBox {background-color:white;font-weight: bold;border-radius: 4px;padding: 4px;margin-top: 16px;} QGroupBox::title{font-size:5em;subcontrol-origin: margin;subcontrol-position: top center;padding: 10px 200em 10px 200em;background-color: white;}");
    settingsLayout->addWidget(rappelBox);
    QGridLayout *rappelLayout = new QGridLayout(rappelBox);

    label = new QLabel("Formule utilisée:");
    label->setStyleSheet("background-color:white");
    label->setStyleSheet("background-color:white");
    rappelLayout->addWidget(label,1,0);

    label = new QLabel;
    label->setStyleSheet("background-color:white");label->setStyleSheet("background-color:white");
    label->setPixmap(QPixmap("C:/Users/hloic/Documents/UTBM/HM40/HM40-TP-Fitts/data/formule.png").scaled(200,100,Qt::KeepAspectRatio));
    rappelLayout->addWidget(label,1,1);

    label = new QLabel("Choix de a et b");
    label->setStyleSheet("background-color:white");label->setStyleSheet("background-color:white");
    rappelLayout->addWidget(label,2,1);

    label = new QLabel("Variable a ");
    label->setStyleSheet("background-color:white");
    rappelLayout->addWidget(label,3,0);
    label = new QLabel("Variable b ");
    label->setStyleSheet("background-color:white");
    rappelLayout->addWidget(label,4,0);

    aValue = new QDoubleSpinBox;
    aValue->setValue(this->fittsModel->a);
    rappelLayout->addWidget(aValue,3,1);
    aValue->setStyleSheet("background-color:white;padding : 1em");

    bValue = new QDoubleSpinBox;
    bValue->setValue(this->fittsModel->b);
    rappelLayout->addWidget(bValue,4,1);
    bValue->setStyleSheet("background-color:white;padding : 1em");

    QGroupBox *configBox = new QGroupBox("Configuration du test :");
    configBox->setStyleSheet("QGroupBox {background-color:white;font-weight: bold;border-radius: 4px;padding: 4px;margin-top: 16px;} QGroupBox::title{font-size:5em;subcontrol-origin: margin;subcontrol-position: top center;padding: 10px 200em 10px 200em;background-color: white;}");
    settingsLayout->addWidget(configBox);
    QGridLayout *configLayout = new QGridLayout(configBox);

    label = new QLabel("Nombre de cibles:");
    label->setStyleSheet("background-color:white");
    configLayout->addWidget(label,1,0);
    label = new QLabel("Taille minimum cible:");
    label->setStyleSheet("background-color:white");
    configLayout->addWidget(label,2,0);
    label = new QLabel("Taille maximal cible:");
    label->setStyleSheet("background-color:white");
    configLayout->addWidget(label,3,0);

    nbCible = new QSpinBox;
    nbCible->setValue(this->fittsModel->nbCible);
    nbCible->setMaximum(100);
    nbCible->setStyleSheet("background-color:white;padding : 1em");
    configLayout->addWidget(nbCible,1,1);

    minSize = new QSpinBox;
    minSize->setMaximum(1000);
    minSize->setValue(this->fittsModel->minSize);
    minSize->setStyleSheet("background-color:white;padding : 1em");
    configLayout->addWidget(minSize,2,1);

    maxSize = new QSpinBox;
    maxSize->setMaximum(1000);
    maxSize->setValue(this->fittsModel->maxSize);
    maxSize->setStyleSheet("background-color:white;padding : 1em");
    configLayout->addWidget(maxSize,3,1);


    QHBoxLayout *btnLayout = new QHBoxLayout;
    settingsLayout->addLayout(btnLayout);
    leaveBtn = new QPushButton("Quitter");
    QPixmap pixmap("C:/Users/hloic/Documents/UTBM/HM40/HM40-TP-Fitts/data/quit.png");
    QIcon ButtonIcon(pixmap);
    leaveBtn->setIcon(ButtonIcon);
    leaveBtn->setIconSize(leaveBtn->size()/15);
    leaveBtn->setStyleSheet("font-size: 30px;display:inline-block;padding:0.7em 1.2em;margin:0 0.3em 0.3em 0;border-radius:0.2em;box-sizing: border-box;text-decoration:none;font-family:'Roboto',sans-serif;font-weight:300;color:#FFFFFF;background-color:#E53935;text-align:center;");
    btnLayout->addWidget(leaveBtn);

    startBtn = new QPushButton("Démarrer");
    QPixmap pixmap2("C:/Users/hloic/Documents/UTBM/HM40/HM40-TP-Fitts/data/start.png");
    QIcon ButtonIcon2(pixmap2);
    startBtn->setIcon(ButtonIcon2);
    startBtn->setIconSize(startBtn->size()/15);
    startBtn->setStyleSheet("font-size: 30px;display:inline-block;padding:0.7em 1.2em;margin:0 0.3em 0.3em 0;border-radius:0.2em;box-sizing: border-box;text-decoration:none;font-family:'Roboto',sans-serif;font-weight:300;color:#FFFFFF;background-color:#2ECC71;text-align:center;");
    btnLayout->addWidget(startBtn);


    // Test part
    QWidget *testWidget = new QWidget;
    mainStack->addWidget(testWidget);

    QVBoxLayout *testLayout = new QVBoxLayout(testWidget);

    testLabel = new QLabel;
    testLayout->addWidget(testLabel);

    graphicView = new GraphicWidget;
    testLayout->addWidget(graphicView);
    graphicView->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff);
    graphicView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scene = new QGraphicsScene;
    graphicView->fitInView(scene->sceneRect(),Qt::KeepAspectRatio);
    graphicView->setScene(scene);
    scene->setSceneRect(0,0,graphicView->width(),300);

    label = new QLabel("Cliquez sur les cibles qui apparaissent en <font color='red'><strong>rouge</strong></font>");
    testLayout->addWidget(label);

    btnLayout = new QHBoxLayout;
    testLayout->addLayout(btnLayout);

    backBtn = new QPushButton("Retour");
    backBtn->setStyleSheet("font-size: 30px;display:inline-block;padding:0.7em 1.2em;margin:0 0.3em 0.3em 0;border-radius:0.2em;box-sizing: border-box;text-decoration:none;font-family:'Roboto',sans-serif;font-weight:300;color:#FFFFFF;background-color:#2980B9;text-align:center;");
    btnLayout->addWidget(backBtn);

    resultBtn = new QPushButton("Résultats");
    resultBtn->setStyleSheet("font-size: 30px;display:inline-block;padding:0.7em 1.2em;margin:0 0.3em 0.3em 0;border-radius:0.2em;box-sizing: border-box;text-decoration:none;font-family:'Roboto',sans-serif;font-weight:300;color:#FFFFFF;background-color:#2980B9;text-align:center;");
    btnLayout->addWidget(resultBtn);
    resultBtn->setEnabled(false);

    QWidget *resultWidget = new QWidget;
    mainStack->addWidget(resultWidget);

    QVBoxLayout *resultLayout = new QVBoxLayout(resultWidget);

    plot = new QChartView;
    resultLayout->addWidget(plot);

    QGroupBox *resultBox =  new QGroupBox("Stats");
    resultLayout->addWidget(resultBox);
    QGridLayout *resultBoxLayout = new QGridLayout(resultBox);

    label = new QLabel("Ecart-Type : ");
    resultBoxLayout->addWidget(label,0,0);
    ecartType = new QLabel;
    resultBoxLayout->addWidget(ecartType,0,1);

    label = new QLabel("Erreur-Type : ");
    resultBoxLayout->addWidget(label,1,0);
    erreurType = new QLabel;
    resultBoxLayout->addWidget(erreurType,1,1);

    label = new QLabel("Différence moyenne : ");
    resultBoxLayout->addWidget(label,0,2);
    diffMoy = new QLabel;
    resultBoxLayout->addWidget(diffMoy,0,3);

    label = new QLabel("Intervalle de confiance à 95% : ");
    resultBoxLayout->addWidget(label,1,2);
    itc95 = new QLabel;
    resultBoxLayout->addWidget(itc95,1,3);

    resultBoxLayout->setColumnStretch(1,10);
    resultBoxLayout->setColumnStretch(3,10);

    btnLayout = new QHBoxLayout;
    resultLayout->addLayout(btnLayout);

    resultLeaveBtn = new QPushButton("Quitter");
    resultLeaveBtn->setStyleSheet("font-size: 30px;display:inline-block;padding:0.7em 1.2em;margin:0 0.3em 0.3em 0;border-radius:0.2em;box-sizing: border-box;text-decoration:none;font-family:'Roboto',sans-serif;font-weight:300;color:#FFFFFF;background-color:#E53935;text-align:center;");
    btnLayout->addWidget(resultLeaveBtn);

    restartBtn = new QPushButton("Recommencer");
    restartBtn->setStyleSheet("font-size: 30px;display:inline-block;padding:0.7em 1.2em;margin:0 0.3em 0.3em 0;border-radius:0.2em;box-sizing: border-box;text-decoration:none;font-family:'Roboto',sans-serif;font-weight:300;color:#FFFFFF;background-color:#2ECC71;text-align:center;");
    btnLayout->addWidget(restartBtn);
}

void FittsView::updateTestMsg() {
    this->testLabel->setText("<strong>Le test commencera après avoir appuyé sur la cible bleu.</strong> Nombre de cibles restantes : " + QString::number(this->fittsModel->cibleLeft));
}


void FittsView::displayResults() {
    this->diffMoy->setText(QString::number(this->fittsModel->diffMoy));
    this->ecartType->setText(QString::number(this->fittsModel->ecartType));
    this->erreurType->setText(QString::number(this->fittsModel->erreurType));
    this->itc95->setText(QString::number(this->fittsModel->itc95));
}
