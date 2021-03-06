#include "electronicsmainwindow.h"
#include "ui_electronicsmainwindow.h"

const QString ElectronicsMainWindow::file_initial = "file:///";
const QString ElectronicsMainWindow::html = ".html";

ElectronicsMainWindow::ElectronicsMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ElectronicsMainWindow)
{
    ui->setupUi(this);
    ui->setupUi(this);
    layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);


    //Connect more button with searchgroupbox
    connect(ui->moreButton, SIGNAL(toggled(bool)), ui->searchGroupBox, SLOT(setVisible(bool)));

    //Connecting Search Line Edit with topic finder
    connect(ui->searchEdit, SIGNAL(textEdited(QString)), this, SLOT(findTopic(QString)));
    //Connecting topic finder with updating index list
    connect(this, SIGNAL(passFoundTopicList(QStringList)), this, SLOT(updateIndexList(QStringList)));
    //Connecting key with the link loader (WebView)
    connect(this, SIGNAL(passKey(int)), this, SLOT(loadLink(int)));
    connect(this, SIGNAL(loadUrl(QUrl)), this, SLOT(loadTheUrl(QUrl)));


    //Adding Items to the list
    loadListDefaultItems();
    //Initially searchGroupBox is invisible
    ui->searchGroupBox->setVisible(false);
    loadTopicLink();
    //WebView Loads first item on the tree

    QUrl theUrl(file_initial + "baseconvert-ui-master/index" + html);

    qDebug() << theUrl;
    //ui->webView->load(topic_name_link_map.value(DEFAULT_INDEX).second);
    ui->webView->load(theUrl);
    ui->indexTreeWidget->setAnimated(true);
    ui->moreButton->setText("More");
    ui->moreButton->setIcon(QIcon(":/images/left_arrow.svg"));
    ui->fontButton->setIcon(QIcon(":/images/fontsizeicon.svg"));
}

ElectronicsMainWindow::~ElectronicsMainWindow()
{
    delete ui;
}

void ElectronicsMainWindow::on_moreButton_toggled(bool checked)
{
    ui->searchGroupBox->setVisible(checked);
    if (checked){
        ui->moreButton->setText("Less");
        ui->moreButton->setIcon(QIcon(":/images/right_arrow.svg"));
    } else {
        ui->moreButton->setText("More");
        ui->moreButton->setIcon(QIcon(":/images/left_arrow.svg"));
    }
}

void ElectronicsMainWindow::loadListDefaultItems()
{
    ui->indexListWidget->clear();
    QTreeWidgetItemIterator treeIterator(ui->indexTreeWidget);
    while ((*treeIterator)){
        ui->indexListWidget->addItem((*treeIterator)->text(DEFAULT_TREE_COLUMN));
        ++treeIterator;
    }
}

void ElectronicsMainWindow::loadTopicLink()
{
    //Getting the topics in a list
    QTreeWidgetItemIterator treeIterator(ui->indexTreeWidget);
    while ((*treeIterator)){
        topic_lists.append((*treeIterator)->text(DEFAULT_TREE_COLUMN));
        ++treeIterator;
    }

    //Processing topic name to get generic link
    for (int i = 0; i < topic_lists.size(); i++){
        QString the_link = linkMaker(topic_lists[i]);
        topic_name_link_map.insert(i, qMakePair(topic_lists[i], the_link));
    }

}

QString ElectronicsMainWindow::linkMaker(QString data)
{
    QString link = data;
    link = ((link.toLower()).replace('-','_')).replace(' ','_');
    return  file_initial + link + html;
}


void ElectronicsMainWindow::findTopic(QString the_topic)
{
    found_item_name.clear();
    loadListDefaultItems();
    found_items = ui->indexListWidget->findItems(the_topic, Qt::MatchContains);
    for (QList <QListWidgetItem*>::iterator it = found_items.begin(); it != found_items.end(); ++it){
        found_item_name.append((*it)->text());
    }
    emit passFoundTopicList(found_item_name);
}


void ElectronicsMainWindow::updateIndexList(QStringList list)
{
    ui->indexListWidget->clear();
    ui->indexListWidget->addItems(list);
    if(ui->searchEdit->text() == NULL) loadListDefaultItems();
}

void ElectronicsMainWindow::on_indexTreeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    emit passKey(topic_name_link_map.key(QPair <QString, QString> (item->text(column), linkMaker(item->text(column)))));
}

//Slot loading link
void ElectronicsMainWindow::loadLink(int ind)
{
    ui->webView->stop();
    QUrl url(topic_name_link_map.value(ind).second);
    emit loadUrl(url);
}

//Load item from listwidget
void ElectronicsMainWindow::on_indexListWidget_itemClicked(QListWidgetItem *item)
{
    emit passKey(topic_name_link_map.key(QPair <QString, QString> (item->text(), linkMaker(item->text()))));
}


//Font Settings
void ElectronicsMainWindow::on_fontButton_clicked()
{
    QWebSettings *settings = ui->webView->settings();
    bool *ok = new bool(true);
    QFontDialog *fontDialog = new QFontDialog(this);
    font = fontDialog->getFont(ok, QFont("Helvetica [Cronyx]", 16) , this);

    if (ok){
        settings->setFontFamily(QWebSettings::StandardFont, font.family());
        settings->setFontSize(QWebSettings::MinimumFontSize, font.pointSize());

    } else {
        settings->setFontFamily(QWebSettings::StandardFont, "Helvetica [Cronyx]");
        settings->setFontSize(QWebSettings::MinimumFontSize, 15);
    }
}
void ElectronicsMainWindow::loadTheUrl(QUrl the_url)
{
    ui->webView->load(the_url);
}
