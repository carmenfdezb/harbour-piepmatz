#include "searchmodel.h"

SearchModel::SearchModel(TwitterApi *twitterApi)
    : searchInProgress(false)
{
    this->twitterApi = twitterApi;

    connect(twitterApi, SIGNAL(searchTweetsError(QString)), this, SLOT(handleSearchTweetsError(QString)));
    connect(twitterApi, SIGNAL(searchTweetsSuccessful(QVariantList)), this, SLOT(handleSearchTweetsSuccessful(QVariantList)));
}

int SearchModel::rowCount(const QModelIndex &) const
{
    return searchResults.size();
}

QVariant SearchModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }
    if(role == Qt::DisplayRole) {
        return QVariant(searchResults.value(index.row()));
    }
    return QVariant();
}

void SearchModel::search(const QString &query)
{
    searchInProgress = true;
    QRegExp regex("(\\w+)");
    if (query.contains(regex) || query.isEmpty()) {
        twitterApi->searchTweets(query);
    } else {
        searchInProgress = false;
        emit searchFinished();
    }
}

void SearchModel::handleSearchTweetsSuccessful(const QVariantList &result)
{
    qDebug() << "SearchModel::handleSearchTweetsSuccessful";
    qDebug() << "Result Count: " << QString::number(result.length());

    if (searchInProgress) {
        beginResetModel();
        searchResults.clear();
        searchResults.append(result);
        endResetModel();
        searchInProgress = false;
        emit searchFinished();
    } else {
        qDebug() << "Search API called from somewhere else...";
    }

}

void SearchModel::handleSearchTweetsError(const QString &errorMessage)
{
    if (searchInProgress) {
        searchInProgress = false;
        emit searchError(errorMessage);
    } else {
        qDebug() << "Search API called from somewhere else...";
    }
}
