#include "HTMLItemViewDelegate.h"
#include <QtGui/QStyleOptionViewItemV4>
#include <QtGui/QTextDocument>
#include <QtGui/QAbstractTextDocumentLayout>
#include <QtGui/QPainter>



void HTMLItemViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyleOptionViewItemV4 options = option;
	initStyleOption(&options, index);

	painter->save();

	QTextDocument doc;
	doc.setHtml(options.text);

	options.text = "";
	options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

	QSize iconSize = options.icon.actualSize(options.rect.size());
	painter->translate(options.rect.left()+iconSize.width(), options.rect.top());

	QRect clip(0, 0, options.rect.width()+iconSize.width(), options.rect.height());
	painter->setClipRect(clip);

	QAbstractTextDocumentLayout::PaintContext ctx;

	painter->setFont(options.font);
	doc.documentLayout()->draw(painter, ctx);

	painter->restore();
}


QSize HTMLItemViewDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyleOptionViewItemV4 options = option;
	initStyleOption(&options, index);

	QTextDocument doc;
	doc.setHtml(options.text);
	doc.setTextWidth(options.rect.width());

	return QSize(doc.idealWidth(), doc.size().height());
}
