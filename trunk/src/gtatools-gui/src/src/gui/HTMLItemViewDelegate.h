#ifndef HTMLITEMVIEWDELEGATE_H_
#define HTMLITEMVIEWDELEGATE_H_

#include <QtGui/QStyledItemDelegate>


class HTMLItemViewDelegate : public QStyledItemDelegate
{
public:
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif /* HTMLITEMVIEWDELEGATE_H_ */
