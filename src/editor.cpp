/* This file was part of the SpeedCrunch project
   Copyright (C) 2004,2005 Ariya Hidayat <ariya@kde.org>

   And is now part of abakus.
   Copyright (c) 2005 Michael Pyne <michael.pyne@kdemail.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "function.h"
#include "valuemanager.h"
#include "editor.h"
#include "evaluator.h"
#include "result.h"

#include <qapplication.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3listbox.h>
#include <qpainter.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qstyle.h>
#include <q3syntaxhighlighter.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <qmessagebox.h>
#include <q3vbox.h>
//Added by qt3to4:
#include <QWheelEvent>
#include <QKeyEvent>
#include <Q3Frame>
#include <QMouseEvent>

#include <netwm.h>
#include <fixx11h.h>  // netwm.h includes X11 headers which conflict with qevent
#include <qevent.h>

#include <kdebug.h>

class CalcResultLabel : public QLabel
{
public:
  CalcResultLabel(QWidget *parent, const char *name, int WFlags) :
    QLabel(parent, name, Qt::WFlags)
  {
  }

protected:
  virtual void mousePressEvent(QMouseEvent *)
  {
    hide();
  }
};

class EditorHighlighter : public Q3SyntaxHighlighter
{
public:
  EditorHighlighter( Editor* );
  int highlightParagraph ( const QString & text, int );

private:
  Editor* editor;
};

class Editor::Private
{
public:
  Evaluator* eval;
  QStringList history;
  int index;
  bool autoCompleteEnabled;
  EditorCompletion* completion;
  QTimer* completionTimer;
  bool autoCalcEnabled;
  char format;
  int decimalDigits;
  QTimer* autoCalcTimer;
  QLabel* autoCalcLabel;
  bool syntaxHighlightEnabled;
  EditorHighlighter* highlighter;
  QMap<ColorType,QColor> highlightColors;
  QTimer* matchingTimer;
};

class EditorCompletion::Private
{
public:
  Editor* editor;
  Q3VBox *completionPopup;
  Q3ListBox *completionListBox;
};

class ChoiceItem: public Q3ListBoxText
{
  public:
    ChoiceItem( Q3ListBox*, const QString& );
    void setMinNameWidth (int w) { minNameWidth = w; }
    int nameWidth() const;

  protected:
    void paint( QPainter* p );

  private:
    QString item;
    QString desc;
    int minNameWidth;
};

ChoiceItem::ChoiceItem( Q3ListBox* listBox, const QString& text ):
  Q3ListBoxText( listBox, text ), minNameWidth(0)
{
  QStringList list = QStringList::split( ':', text );
  if( list.count() )  item = list[0];
  if( list.count()>1 )  desc = list[1];
}

// Returns width of this particular list item's name.
int ChoiceItem::nameWidth() const
{
  if(item.isEmpty())
    return 0;

  QFontMetrics fm = listBox()->fontMetrics();
  return fm.width( item );
}

void ChoiceItem::paint( QPainter* painter )
{
  int itemHeight = height( listBox() );
  QFontMetrics fm = painter->fontMetrics();
  int yPos = ( ( itemHeight - fm.height() ) / 2 ) + fm.ascent();
  painter->drawText( 3, yPos, item );

  //int xPos = fm.width( item );
  int xPos = QMAX(fm.width(item), minNameWidth);
  if( !isSelected() )
    painter->setPen( listBox()->palette().disabled().text().dark() );
  painter->drawText( 10 + xPos, yPos, desc );
}

EditorHighlighter::EditorHighlighter( Editor* e ):
  Q3SyntaxHighlighter( e )
{
  editor = e;
}

int EditorHighlighter::highlightParagraph ( const QString & text, int )
{
  if( !editor->isSyntaxHighlightEnabled() )
  {
    setFormat( 0, text.length(), editor->colorGroup().text() );
    return 0;
  }

  QStringList fnames = FunctionManager::instance()->functionList(FunctionManager::All);
  fnames.sort(); // Sort list so we can bin search it.

  Tokens tokens = Evaluator::scan( text );
  for( unsigned i = 0; i < tokens.count(); i++ )
  {
    Token& token = tokens[i];
    QString text = token.text().lower();
    QFont font = editor->font();
    QColor color = Qt::black;
    switch( token.type() )
    {
      case Token::Number:
        color = editor->highlightColor( Editor::Number );
        break;

      case Token::Identifier:
        {
          color = editor->highlightColor( Editor::Variable );

// XXX: QT 4: Replace this with qBinaryFind().

          if( fnames.contains( text ) ) {
            color = editor->highlightColor( Editor::FunctionName );
          }
        }
        break;

      case Token::Operator:
        break;

      default: break;
    };
    if( token.pos() >= 0 ) {
      setFormat( token.pos(), token.text().length(), font, color );
    }
  }
  return 0;
}



Editor::Editor( QWidget* parent, const char* name ):
  Q3TextEdit( parent, name )
{
  d = new Private;
  d->eval = 0;
  d->index = 0;
  d->autoCompleteEnabled = true;
  d->completion = new EditorCompletion( this );
  d->completionTimer = new QTimer( this );
  d->autoCalcEnabled = true;
  d->syntaxHighlightEnabled = true;
  d->highlighter = new EditorHighlighter( this );
  d->autoCalcTimer = new QTimer( this );
  d->matchingTimer = new QTimer( this );

  setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  setWordWrap( NoWrap );
  setHScrollBarMode( AlwaysOff );
  setVScrollBarMode( AlwaysOff );
  setTextFormat( Qt::PlainText );
  setAutoFormatting( AutoNone );
  setTabChangesFocus( true );
  setLinkUnderline( false );

  connect( d->completion, SIGNAL( selectedCompletion( const QString& ) ),
    SLOT( autoComplete( const QString& ) ) );
  connect( this, SIGNAL( textChanged() ), SLOT( checkAutoComplete() ) );
  connect( d->completionTimer, SIGNAL( timeout() ), SLOT( triggerAutoComplete() ) );

  connect( this, SIGNAL( textChanged() ), SLOT( checkMatching() ) );
  connect( d->matchingTimer, SIGNAL( timeout() ), SLOT( doMatchingLeft() ) );
  connect( d->matchingTimer, SIGNAL( timeout() ), SLOT( doMatchingRight() ) );
  connect( this, SIGNAL( textChanged() ), SLOT( checkAutoCalc() ) );
  connect( d->autoCalcTimer, SIGNAL( timeout() ), SLOT( autoCalc() ) );
  d->autoCalcLabel = new CalcResultLabel( 0, "autocalc", Qt::WStyle_StaysOnTop |
    Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool |  Qt::WX11BypassWM );
  d->autoCalcLabel->setFrameStyle( Q3Frame::Plain | Q3Frame::Box );
  d->autoCalcLabel->setPalette( QToolTip::palette() );
  d->autoCalcLabel->hide();

  setHighlightColor( Number, QColor(0,0,127) );
  setHighlightColor( FunctionName, QColor(85,0,0) );
  setHighlightColor( Variable, QColor(0,85,0) );
  setHighlightColor( MatchedPar, QColor(255,255,183) );
}

Editor::~Editor()
{
  d->autoCalcLabel->hide();
  delete d;
}

QSize Editor::sizeHint() const
{
  constPolish();
  QFontMetrics fm = fontMetrics();
  int h = QMAX(fm.lineSpacing(), 14);
  int w = fm.width( 'x' ) * 20;
  int m = frameWidth() * 2;
  return( style().sizeFromContents(QStyle::CT_LineEdit, this,
             QSize( w + m, h + m + 4 ).
             expandedTo(QApplication::globalStrut())));
}

QStringList Editor::history() const
{
  return d->history;
}

void Editor::setHistory( const QStringList& h )
{
  d->history = h;
  d->index = d->history.count();
}

bool Editor::autoCompleteEnabled() const
{
  return d->autoCompleteEnabled;
}

void Editor::setAutoCompleteEnabled( bool enable )
{
  d->autoCompleteEnabled = enable;
}

bool Editor::autoCalcEnabled() const
{
  return d->autoCalcEnabled;
}

void Editor::setAutoCalcEnabled( bool enable )
{
  d->autoCalcEnabled = enable;
}

void Editor::setFormat( char format )
{
  d->format = format;
}

void Editor::setDecimalDigits( int digits )
{
  d->decimalDigits = digits;
}

void Editor::appendHistory( const QString& text )
{
  if( text.isEmpty() ) return;

  QString lastText;
  if( d->history.count() )
    lastText = d->history[ d->history.count()-1 ];
  if( text == lastText ) return;

  d->history.append( text );
  d->index = d->history.count()-1;
}

void Editor::clearHistory()
{
  d->history.clear();
  d->index = 0;
}

void Editor::squelchNextAutoCalc()
{
  d->autoCalcTimer->stop();
}

void Editor::setText(const QString &txt)
{
  Q3TextEdit::setText(txt);
  squelchNextAutoCalc();
}

void Editor::checkAutoComplete()
{
  if( !d->autoCompleteEnabled ) return;

  d->completionTimer->stop();
  d->completionTimer->start( 500, true );
}

void Editor::checkMatching()
{
  if( !d->syntaxHighlightEnabled ) return;

  d->matchingTimer->stop();
  d->matchingTimer->start( 200, true );
}

void Editor::checkAutoCalc()
{
  // Calc-As-You-Type
  if( !d->autoCalcEnabled ) return;

  d->autoCalcTimer->stop();
  d->autoCalcTimer->start( 1000, true );
  d->autoCalcLabel->hide();
}

void Editor::doMatchingLeft()
{
  if( !d->syntaxHighlightEnabled ) return;

  // tokenize the expression
  int para = 0, curPos = 0;
  getCursorPosition( &para, &curPos );

  // check for right par
  QString subtext = text().left( curPos );
  Tokens tokens = Evaluator::scan( subtext );
  if( !tokens.valid() ) return;
  if( tokens.count()<1 ) return;
  Token lastToken = tokens[ tokens.count()-1 ];

  // right par ?
  if( lastToken.isOperator() )
  if( lastToken.asOperator() == Token::RightPar )
  if( lastToken.pos() == curPos-1 )
  {
    // find the matching left par
    unsigned par = 1;
    int k = 0;
    Token matchToken;
    int matchPos = -1;

    for( k = tokens.count()-2; k >= 0; k-- )
    {
      if( par < 1 ) break;
      Token matchToken = tokens[k];
      if( matchToken.isOperator() )
      {
        if( matchToken.asOperator() == Token::RightPar )
          par++;
        if( matchToken.asOperator() == Token::LeftPar )
          par--;
        if( par == 0 ) matchPos = matchToken.pos();
      }
    }

    if( matchPos >= 0 )
    {
      setSelection( 0, matchPos, 0, matchPos+1, 2 );
      setSelection( 0, lastToken.pos(), 0, lastToken.pos()+1, 1 );
      setCursorPosition( para, curPos );
    }
  }
}

void Editor::doMatchingRight()
{
  if( !d->syntaxHighlightEnabled ) return;

  // tokenize the expression
  int para = 0, curPos = 0;
  getCursorPosition( &para, &curPos );

  // check for left par
  QString subtext = text().right( text().length() - curPos );
  Tokens tokens = Evaluator::scan( subtext );
  if( !tokens.valid() ) return;
  if( tokens.count()<1 ) return;
  Token firstToken = tokens[ 0 ];

  // left par ?
  if( firstToken.isOperator() )
  if( firstToken.asOperator() == Token::LeftPar )
  if( firstToken.pos() == 0 )
  {
    // find the matching right par
    unsigned par = 1;
    unsigned int k = 0;
    Token matchToken;
    int matchPos = -1;

    for( k = 1; k < tokens.count(); k++ )
    {
      if( par < 1 ) break;
      Token matchToken = tokens[k];
      if( matchToken.isOperator() )
      {
        if( matchToken.asOperator() == Token::LeftPar )
          par++;
        if( matchToken.asOperator() == Token::RightPar )
          par--;
        if( par == 0 ) matchPos = matchToken.pos();
      }
    }

    if( matchPos >= 0 )
    {
      setSelection( 0, curPos+matchPos, 0, curPos+matchPos+1, 2 );
      setSelection( 0, curPos+firstToken.pos(), 0, curPos+firstToken.pos()+1, 1 );
      setCursorPosition( para, curPos );
    }
  }

}

void Editor::triggerAutoComplete()
{
  if( !d->autoCompleteEnabled ) return;

  // tokenize the expression (don't worry, this is very fast)
  // faster now that it uses flex. ;)
  int para = 0, curPos = 0;
  getCursorPosition( &para, &curPos );
  QString subtext = text().left( curPos );
  Tokens tokens = Evaluator::scan( subtext );
  if(!tokens.valid())
  {
    kdWarning() << "invalid tokens.\n";
    return;
  }

  if(tokens.isEmpty() || subtext.endsWith(" "))
    return;

  Token lastToken = tokens[ tokens.count()-1 ];

  // last token must be an identifier
  if( !lastToken.isIdentifier() )
    return;

  QString id = lastToken.text();
  if( id.isEmpty() )
    return;

  // find matches in function names
  QStringList fnames = FunctionManager::instance()->functionList(FunctionManager::All);
  QStringList choices;

  for( unsigned i=0; i<fnames.count(); i++ )
    if( fnames[i].startsWith( id, false ) )
    {
      QString str = fnames[i];

      ::Function* f = FunctionManager::instance()->function( str );
      if( f && !f->description.isEmpty() )
        str.append( ':' ).append( f->description );

      choices.append( str );
    }

  choices.sort();

  // find matches in variables names
  QStringList vchoices;
  QStringList values = ValueManager::instance()->valueNames();

  for(QStringList::ConstIterator it = values.begin(); it != values.end(); ++it)
    if( (*it).startsWith( id, false ) )
    {
      QString choice = ValueManager::description(*it);
      if(choice.isEmpty())
        choice = ValueManager::instance()->value(*it).toString();

      vchoices.append( QString("%1:%2").arg( *it, choice ) );
    }

  vchoices.sort();
  choices += vchoices;

  // no match, don't bother with completion
  if( !choices.count() ) return;

  // one match, complete it for the user
  if( choices.count()==1 )
  {
    QString str = QStringList::split( ':', choices[0] )[0];

    // single perfect match, no need to give choices.
    if(str == id.lower())
      return;

    str = str.remove( 0, id.length() );
    int para = 0, curPos = 0;
    getCursorPosition( &para, &curPos );
    blockSignals( true );
    insert( str );
    setSelection( 0, curPos, 0, curPos+str.length() );
    blockSignals( false );
    return;
  }

  // present the user with completion choices
  d->completion->showCompletion( choices );
}

void Editor::autoComplete( const QString& item )
{
  if( !d->autoCompleteEnabled || item.isEmpty() )
    return;

  int para = 0, curPos = 0;
  getCursorPosition( &para, &curPos );

  QString subtext = text().left( curPos );
  Tokens tokens = Evaluator::scan( subtext );

  if( !tokens.valid() || tokens.count() < 1 )
    return;

  Token lastToken = tokens[ tokens.count()-1 ];
  if( !lastToken.isIdentifier() )
    return;

  QStringList str = QStringList::split( ':', item );

  blockSignals( true );
  setSelection( 0, lastToken.pos(), 0, lastToken.pos()+lastToken.text().length() );
  insert( str[0] );
  blockSignals( false );
}

void Editor::autoCalc()
{
  if( !d->autoCalcEnabled )
    return;

  QString str = Evaluator::autoFix( text() );
  if( str.isEmpty() )
    return;

  // too short? do not bother...
  Tokens tokens = Evaluator::scan( str );
  if( tokens.count() < 2 )
    return;

  // If we're using set for a function don't try.
  QRegExp setFn("\\s*set.*\\(.*=");
  if( str.find(setFn) != -1 )
    return;

  // strip off assignment operator, e.g. "x=1+2" becomes "1+2" only
  // the reason is that we want only to evaluate (on the fly) the expression,
  // not to update (put the result in) the variable
  if( tokens.count() > 2 && tokens[0].isIdentifier() &&
      tokens[1].asOperator() == Token::Equal )
  {
    Tokens::const_iterator it = tokens.begin();
    ++it;
    ++it; // Skip first two tokens.

    // Reconstruct string to evaluate using the tokens.
    str = "";
    while(it != tokens.end())
    {
      str += (*it).text();
      str += ' ';
      ++it;
    }
  }

  Abakus::number_t result = parseString(str.latin1());
  if( Result::lastResult()->type() == Result::Value )
  {
    QString ss = QString("Result: <b>%2</b>").arg(result.toString());
    d->autoCalcLabel->setText( ss );
    d->autoCalcLabel->adjustSize();

    // reposition nicely
    QPoint pos = mapToGlobal( QPoint( 0, 0 ) );
    pos.setY( pos.y() - d->autoCalcLabel->height() - 1 );
    d->autoCalcLabel->move( pos );
    d->autoCalcLabel->show();
    d->autoCalcLabel->raise();

    // do not show it forever
    QTimer::singleShot( 5000, d->autoCalcLabel, SLOT( hide()) );
  }
  else
  {
    // invalid expression
    d->autoCalcLabel->hide();
  }
}

QString Editor::formatNumber( const Abakus::number_t &value ) const
{
  return value.toString();
}

void Editor::historyBack()
{
  if( d->history.isEmpty() )
    return;

  d->index--;

  if( d->index < 0 )
    d->index = 0;

  setText( d->history[ d->index ] );
  setCursorPosition( 0, text().length() );
  ensureCursorVisible();
}

void Editor::historyForward()
{
  if( d->history.isEmpty() )
    return;

  d->index++;

  if( d->index >= (int) d->history.count() )
    d->index = d->history.count() - 1;

  setText( d->history[ d->index ] );
  setCursorPosition( 0, text().length() );
  ensureCursorVisible();
}

void Editor::keyPressEvent( QKeyEvent* e )
{
  if( e->key() == Qt::Key_Up )
  {
    historyBack();
    e->accept();
    return;
  }

  if( e->key() == Qt::Key_Down )
  {
    historyForward();
    e->accept();
    return;
  }

  if( e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return )
  {
    emit returnPressed();
    return;
  }

  if( e->key() == Qt::Key_Left ||
      e->key() == Qt::Key_Right ||
      e->key() == Qt::Key_Home ||
      e->key() == Qt::Key_End )
  {
    checkMatching();
  }

  Q3TextEdit::keyPressEvent( e );
}

void Editor::wheelEvent( QWheelEvent *e )
{
  if( e->delta() > 0 )
    historyBack();
  else if( e->delta() < 0 )
    historyForward();

  e->accept();
}

void Editor::setSyntaxHighlight( bool enable )
{
  d->syntaxHighlightEnabled = enable;
  d->highlighter->rehighlight();
}

bool Editor::isSyntaxHighlightEnabled() const
{
  return d->syntaxHighlightEnabled;
}

void Editor::setHighlightColor( ColorType type, QColor color )
{
  d->highlightColors[ type ] = color;

  setSelectionAttributes( 1, highlightColor( Editor::MatchedPar ), false );
  setSelectionAttributes( 2, highlightColor( Editor::MatchedPar ), false );

  d->highlighter->rehighlight();
}

QColor Editor::highlightColor( ColorType type )
{
  return d->highlightColors[ type ];
}


EditorCompletion::EditorCompletion( Editor* editor ): QObject( editor )
{
  d = new Private;
  d->editor = editor;

  d->completionPopup = new Q3VBox( editor->topLevelWidget(), 0, Qt::WType_Popup );
  d->completionPopup->setFrameStyle( Q3Frame::Box | Q3Frame::Plain );
  d->completionPopup->setLineWidth( 1 );
  d->completionPopup->installEventFilter( this );
  d->completionPopup->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Minimum);

  d->completionListBox = new Q3ListBox( d->completionPopup );
  d->completionPopup->setFocusProxy( d->completionListBox );
  d->completionListBox->setFrameStyle( Q3Frame::NoFrame );
  d->completionListBox->setVariableWidth( true );
  d->completionListBox->installEventFilter( this );
}

EditorCompletion::~EditorCompletion()
{
  delete d;
}

bool EditorCompletion::eventFilter( QObject *obj, QEvent *ev )
{
  if ( obj == d->completionPopup || obj == d->completionListBox )
  {

    if ( ev->type() == QEvent::KeyPress )
    {
      QKeyEvent *ke = (QKeyEvent*)ev;
      if ( ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return  )
      {
        doneCompletion();
        return true;
      }
      else if ( ke->key() == Qt::Key_Left || ke->key() == Qt::Key_Right ||
      ke->key() == Qt::Key_Up || ke->key() == Qt::Key_Down ||
      ke->key() == Qt::Key_Home || ke->key() == Qt::Key_End ||
      ke->key() == Qt::Key_PageUp || ke->key() == Qt::Key_PageDown )
        return false;

      d->completionPopup->close();
      d->editor->setFocus();
      QApplication::sendEvent( d->editor, ev );
      return true;
    }

    if ( ev->type() == QEvent::MouseButtonDblClick )
    {
      doneCompletion();
      return true;
    }

  }

  return false;
}

void EditorCompletion::doneCompletion()
{
  d->completionPopup->close();
  d->editor->setFocus();
  emit selectedCompletion( d->completionListBox->currentText() );
}

void EditorCompletion::showCompletion( const QStringList &choices )
{
  static bool shown = false;
  if( !choices.count() ) return;

  d->completionListBox->clear();
  int maxWidth = 0;
  for( unsigned i = 0; i < choices.count(); i++ ) {
    ChoiceItem *item = new ChoiceItem( d->completionListBox, choices[i] );
    int itemMaxWidth = item->nameWidth();

    if(itemMaxWidth > maxWidth)
      maxWidth = itemMaxWidth;
  }

  for(unsigned i = 0; i < d->completionListBox->count(); ++i) {
    ChoiceItem *item = static_cast<ChoiceItem *>(d->completionListBox->item(i));
    item->setMinNameWidth(maxWidth);
  }

  d->completionListBox->setCurrentItem( 0 );

  // size of the pop-up
  d->completionPopup->setMaximumHeight( 120 );
  d->completionPopup->resize( d->completionListBox->sizeHint() +
           QSize( d->completionListBox->verticalScrollBar()->width() + 4,
            d->completionListBox->horizontalScrollBar()->height() + 4 ) );

  if(!shown)
  {
    d->completionPopup->show();
    QTimer::singleShot ( 0, this, SLOT(moveCompletionPopup()) );
  }
  else
  {
    moveCompletionPopup();
    d->completionPopup->show();
  }
}

void EditorCompletion::moveCompletionPopup()
{
  int h = d->completionListBox->height();
  int w = d->completionListBox->width();

  // position, reference is editor's cursor position in global coord
  QFontMetrics fm( d->editor->font() );
  int para = 0, curPos = 0;

  d->editor->getCursorPosition( &para, &curPos );

  int pixelsOffset = fm.width( d->editor->text(), curPos );
  pixelsOffset -= d->editor->contentsX();
  QPoint pos = d->editor->mapToGlobal( QPoint( pixelsOffset, d->editor->height() ) );

  // if popup is partially invisible, move to other position
  NETRootInfo info(d->completionPopup->x11Display(),
	  NET::CurrentDesktop | NET::WorkArea | NET::NumberOfDesktops,
	  -1, false);
  info.activate(); // wtf is this needed for?
  NETRect NETarea = info.workArea(info.currentDesktop());

  QRect area(NETarea.pos.x, NETarea.pos.y, NETarea.size.width, NETarea.size.height);

  if( pos.y() + h > area.y() + area.height() )
    pos.setY( pos.y() - h - d->editor->height() );
  if( pos.x() + w > area.x() + area.width() )
    pos.setX( area.x() + area.width() - w );

  d->completionPopup->move( pos );
  d->completionListBox->setFocus();
}

#include "editor.moc"

// vim: set et sw=2 ts=8:
