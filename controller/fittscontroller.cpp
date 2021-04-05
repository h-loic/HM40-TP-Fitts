#include "fittscontroller.h"
#include "../View/fittsview.h"
#include "../Model/fittsmodel.h"

#include <QApplication>
#include <QDebug>
#include <QtGlobal>
#include <QChart>
#include <QLineSeries>
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QLegend>
#include <QBarCategoryAxis>
#include <QHorizontalBarSeries>
#include <QCategoryAxis>
#include <QPieSeries>
#include <QPieSlice>
#include <QStringList>
#include <QElapsedTimer>

QT_CHARTS_USE_NAMESPACE

FittsController::FittsController(FittsView *fittsView, FittsModel *fittsModel) {
    this->fittsModel = fittsModel;
    this->fittsView = fittsView;
    this->start();
}

void FittsController::start() {
    this->fittsView->show();
}

void FittsController::startSimulation() {
    this->fittsView->mainStack->setCurrentIndex(1);
    this->fittsModel->cibleLeft = this->fittsModel->nbCible;
    this->fittsView->updateTestMsg();
    this->fittsView->resultBtn->setEnabled(false);
    this->fittsView->graphicView->setEnabled(true);
    this->fittsModel->cercleSize.clear();
    this->fittsModel->cercleCenter.clear();
    this->fittsModel->clickPoints.clear();
    this->fittsModel->times.clear();

    this->initGame();
}

void FittsController::quit() {
    QApplication::quit();
}

void FittsController::backToSettings() {
    this->fittsView->mainStack->setCurrentIndex(0);
}
void FittsController::resultClicked() {
    this->fittsView->mainStack->setCurrentIndex(2);

    this->calculateResult();
}
void FittsController::aValueChanged(double value) {
    this->fittsModel->a = value;
}
void FittsController::bValueChanged(double value) {
    this->fittsModel->b = value;
}
void FittsController::nbCibleChanged(int value) {
    this->fittsModel->nbCible = value;
}
void FittsController::minSizeChanged(int value) {
    this->fittsModel->minSize = value;
}
void FittsController::maxSizeChanged(int value) {
    this->fittsModel->maxSize = value;
}
void FittsController::cibleClicked(int x, int y) {
    if(this->fittsModel->cercleCenter.isEmpty()) {
        // Si vide alors premier click, on demarre le timer
        this->timer = new QElapsedTimer;
        timer->start();

        // On démarre avec la première cible
        this->fittsModel->clickPoints.append(QPoint(x,y));
        this->nextCible();
    }
    else {
        QPointF coords = this->fittsView->graphicView->mapToScene(x,y);
        if(sqrt(pow(coords.x() - this->fittsModel->cercleCenter.last().x(),2) + pow(coords.y() - this->fittsModel->cercleCenter.last().y(),2)) <= this->fittsModel->cercleSize.last() / 2) {
            // On stock le temps de click
            this->fittsModel->times.append(timer->elapsed());
            // On restart le chrono
            timer->restart();

            // On stock la position du click
            this->fittsModel->clickPoints.append(QPoint(x,y));
            this->nextCible();
        }
    }
}

void FittsController::nextCible() {
    if(!this->fittsModel->cercleCenter.isEmpty())
        this->fittsModel->cibleLeft--;
    this->fittsView->updateTestMsg();

    QGraphicsScene *scene = this->fittsView->scene;
    scene->clear();

    // On stop si c'est finis
    if(this->fittsModel->cibleLeft == 0) {
        this->finish();
        return;
    }

    // On génère la taille du cercle rouge
    // qrand() % ((high + 1) - low) + low;
    int size = qrand() % ((this->fittsModel->maxSize + 1) - this->fittsModel->minSize) + this->fittsModel->minSize;
    // Car on veut le rayon
    // On place le cercle dans la scene (Attention faut pas qu'il soit en dehors du cadre)
    int sceneW = int(this->fittsView->scene->width());
    int sceneH = int(this->fittsView->scene->height());

    qreal posX = qrand() % ((sceneW - size) - size) + size;
    qreal posY = qrand() % ((sceneH - size) - size) + size;

    // On stock les infos sur le cercle
    this->fittsModel->cercleCenter.append(QPoint(int(posX),int(posY)));
    this->fittsModel->cercleSize.append(size);

    // On place le cercle
    scene->addEllipse(posX - (size / 2), posY - (size / 2), size, size, QPen(QColor("red")),QBrush(QColor("red")));
}


void FittsController::finish() {
    this->fittsView->graphicView->setEnabled(false);
    this->fittsView->resultBtn->setEnabled(true);
}

void FittsController::initGame() {
    QGraphicsScene *scene = this->fittsView->scene;
    scene->clear();

    if(this->fittsModel->maxSize >= this->fittsView->graphicView->width() / 2)
        this->fittsModel->maxSize = this->fittsView->graphicView->width() / 2;

    if(this->fittsModel->maxSize >= this->fittsView->graphicView->height() / 2)
        this->fittsModel->maxSize = this->fittsView->graphicView->height() / 2;

    qreal posX = scene->width() / 2;
    qreal posY = scene->height() / 2;
    int size = 100;

    scene->addEllipse(posX - (size / 2), posY - (size / 2), size, size, QPen(QColor("blue")),QBrush(QColor("blue")));
}

void FittsController::calculateResult() {
        QChart *graphFitts = new QChart;
        QChart *graphdistance = new QChart;

        this->fittsView->chartFitts->setChart(graphFitts);
        this->fittsView->chartFitts->setRenderHint(QPainter::Antialiasing);
        this->fittsView->chartRelativeDistance->setChart(graphdistance);
        this->fittsView->chartRelativeDistance->setRenderHint(QPainter::Antialiasing);

        graphFitts->setTitle("Résultats loi Fitts");
        graphFitts->setAnimationOptions(QChart::AllAnimations);
        graphFitts->createDefaultAxes();
        graphFitts->legend()->setVisible(true);
        graphFitts->legend()->setAlignment(Qt::AlignBottom);

        QLineSeries *expSeries = new QLineSeries;
        expSeries->setName("Courbe expérimentale");
        QLineSeries *fittsSeries = new QLineSeries;
        fittsSeries->setName("Courbe théorique");
        QCategoryAxis *axis = new QCategoryAxis;

        QList<double> fittsValues;


        QLineSeries *expSeries2 = new QLineSeries;
        expSeries2->setName("Données expérimentales");
        QLineSeries *fittsSeries2 = new QLineSeries;
        fittsSeries2->setName("Courbe théorique");
        QCategoryAxis *axis2 = new QCategoryAxis;
        graphdistance->setTitle("Résultat en fonction du temps et de la distance");
        graphdistance->setAnimationOptions(QChart::AllAnimations);
        graphdistance->createDefaultAxes();
        graphdistance->legend()->setVisible(true);
        graphdistance->legend()->setAlignment(Qt::AlignBottom);


        QList<double> fittsValues2;

        for(int i = 0; i < this->fittsModel->nbCible; ++i) {
            double T = this->fittsModel->times[i];
            expSeries->append(i,T);
            double D = sqrt(pow(this->fittsModel->clickPoints[i].x() - this->fittsModel->cercleCenter[i].x(),2) + pow(this->fittsModel->clickPoints[i].y() - this->fittsModel->cercleCenter[i].y(),2));
            double L = this->fittsModel->cercleSize[i];
            double F = log2(2*D/L);
            expSeries2->append(F,T);

            // On multiplie par 100 pour être en ms
            double value = (this->fittsModel->a * 1000) + ((this->fittsModel->b * 1000) * log2((D / this->fittsModel->cercleSize[i]) + 1));
            fittsValues.append(value);
            fittsSeries->append(i,value);
            fittsSeries2->append(F,value);
            axis->append(QString::number(i + 1), i);
            axis2->append(QString::number(F),i);
        }

        QVector<QPointF> points = fittsSeries2->pointsVector();
        std::sort(points.begin(), points.end(), [](const QPointF & p1, const QPointF & p2) {
            return p1.y() > p2.y();
        });
        fittsSeries2->replace(points);

        QVector<QPointF> points2 = expSeries2->pointsVector();
        std::sort(points2.begin(), points2.end(), [](const QPointF & p21, const QPointF & p22) {
            return p21.y() > p22.y();
        });
        expSeries2->replace(points2);

        axis->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
        graphFitts->addSeries(expSeries);
        graphFitts->addSeries(fittsSeries);
        graphFitts->setAxisX(axis,expSeries);
        graphFitts->setAxisX(axis,fittsSeries);
        QValueAxis *axisY = new QValueAxis;
        QValueAxis *axisX = new QValueAxis;
        axisY->setTitleText("temps (en ms)");
        axisX->setTitleText("numéro de la cible");
        graphFitts->setAxisY(axisY,expSeries);

        axis2->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
        graphdistance->addSeries(expSeries2);
        graphdistance->addSeries(fittsSeries2);
        graphdistance->setAxisX(axis2,expSeries2);
        graphdistance->setAxisX(axis2,fittsSeries2);
        QValueAxis *axisY2 = new QValueAxis;
        QValueAxis *axisX2 = new QValueAxis;
        axisY2->setTitleText("temps (en ms)");
        axisX2->setTitleText("log(2D/L)");
        graphdistance->setAxisY(axisY2,expSeries2);
        graphdistance->setAxisX(axisX2,expSeries2);

    // Calcul des valeurs
    // Moyennes
    QList<double> diffValues;
    double diffMoy = 0;

    for (int i = 0; i < fittsValues.size(); ++i) {
        diffValues.append(fabs(fittsValues[i] - this->fittsModel->times[i]));
        diffMoy += fabs(fittsValues[i] - this->fittsModel->times[i]);
    }
    diffMoy /= fittsValues.size();

    // On stock la difference de moyenne
    this->fittsModel->diffMoy = fabs(diffMoy);


    // Ecart type
    double variance = 0;

    for (int i = 0; i < fittsValues.size(); ++i) {
        variance += pow(diffValues[i] - diffMoy,2);
    }
    variance /= fittsValues.size();

    double ecartType = sqrt(variance);

    // On stock l'ecart type
    this->fittsModel->ecartType = ecartType;
    // On stock l'erreur type
    this->fittsModel->erreurType = fabs(ecartType / sqrt(fittsValues.size()));

    // On stock itc 95%
    this->fittsModel->itc95 = 2 * this->fittsModel->erreurType;

    this->fittsView->displayResults();
}
