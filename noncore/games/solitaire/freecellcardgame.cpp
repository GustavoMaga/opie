/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/
#include <qgfx_qws.h>
#include "freecellcardgame.h"


extern int highestZ;
int numberOfFreeCells = 4;


FreecellCardGame::FreecellCardGame(QCanvas *c, bool snap, QWidget *parent) : CanvasCardGame(*c, snap, parent)
{
    numberOfFreeCells = 4;
    highestZ = 0;

    int spaceBetweenPiles = ( qt_screen->deviceWidth() < 200 ) ? 21 : 28;
    int xOrigin = ( qt_screen->deviceWidth() < 200 ) ? 0 : 5;
    int spacing = ( qt_screen->deviceWidth() < 200 ) ? 0 : 0;

    for (int i = 0; i < 4; i++) {
	freecellPiles[i] = new FreecellFreecellPile( xOrigin + i * spaceBetweenPiles, 10, canvas() );
	addCardPile(freecellPiles[i]);
    }
    for (int i = 0; i < 4; i++) {
	discardPiles[i] = new FreecellDiscardPile( xOrigin + spacing + 6 + (i + 4) * spaceBetweenPiles, 10, canvas() );
	addCardPile(discardPiles[i]);
    }
    for (int i = 0; i < 8; i++) {
	workingPiles[i] = new FreecellWorkingPile( xOrigin + spacing + 2 + i * spaceBetweenPiles, 50, canvas() );
	addCardPile(workingPiles[i]);
    }
}


void FreecellCardGame::deal(void)
{
    highestZ = 1;
    
    beginDealing();
    
    for (int i = 0; i < 52; i++) {
	Card *card = cards[i];
	card->setFace( TRUE );
	card->setPos( 0, 0, highestZ );
	card->setCardPile( workingPiles[i%8] );
	workingPiles[i%8]->addCardToTop( card );
	card->move( workingPiles[i%8]->getCardPos( card ) );
	card->showCard();
	highestZ++;
    }

    endDealing();
}

// 	checks if smaller card with different color, that could be put on top on the
// 	card, is present in working or freecell pile
bool FreecellCardGame::checkNeeded(Card *card)
{
	if (card->getValue() > 2){
		int i;
		Card *c;
		for (i=0;i<4;i++){
			c = freecellPiles[i]->cardOnBottom();
			if (c != NULL){
				if (card->isRed()!= c->isRed() && card->getValue()== c->getValue()+1){
					return (false); 
				}
			}
		}
		for (i=0;i<8;i++){
			c=workingPiles[i]->cardOnBottom();
			while (c!=NULL){
				if (card->isRed() != c->isRed()	&& card->getValue() == c->getValue()+1) {
					return (false);
				}
				c=workingPiles[i]->cardInfront(c);
			}
		}
	}
	return(true);
}
	
//  added to move cards, on which no card can be moved, to discard pile 
void FreecellCardGame::checkUnusable()
{
	int i,j;
//	printf("void FreecellCardGame::checkUnusable()\n");
	Card *top_one;
	for (i=0;i < 8;i++)
	{
		top_one = workingPiles[i]->cardOnTop();
		if (top_one != NULL)
		{
			j = 0;
			while ((j < 4))
			{
				if (discardPiles[j]->isAllowedOnTop(top_one)){
					if (checkNeeded(top_one)){
						top_one->setCardPile(discardPiles[j]);
						workingPiles[i]->removeCard(top_one);
//						printf("k %d f work%d to disk%d on %d\n ",top_one->getValue(),i+1,j+1,highestZ);
						discardPiles[j]->addCardToTop(top_one);
						top_one->setPos(discardPiles[j]->getX(),discardPiles[j]->getY(),highestZ);
						highestZ++;
						j = 4;
						checkUnusable();
					}
				}
				j++;
			}
		} 
	}
	for (i=0;i<4;i++){
		top_one = freecellPiles[i]->cardOnTop();
		if (top_one != NULL)
		{
			j = 0;
			while ((j < 4))
			{
				if (discardPiles[j]->isAllowedOnTop(top_one)){
					if (checkNeeded(top_one)){
						top_one->setCardPile(discardPiles[j]);
						freecellPiles[i]->removeCard(top_one);
//						printf("k %d f work%d to disk%d on %d\n ",top_one->getValue(),i+1,j+1,highestZ);
						discardPiles[j]->addCardToTop(top_one);
						top_one->setPos(discardPiles[j]->getX(),discardPiles[j]->getY(),highestZ);
						highestZ++;
						j = 4;
						checkUnusable();
					}
				}
				j++;
			}
		} 
	}		
}


bool FreecellCardGame::mousePressCard( Card *c, QPoint p )
{
    Q_UNUSED(p);
    
    if ( !c->getCardPile()->isAllowedToBeMoved(c) ) {
	moving = NULL;
	return TRUE;
    }

    return FALSE;
}


void FreecellCardGame::readConfig( Config& cfg )
{
    cfg.setGroup("GameState");

    // Create Cards, but don't shuffle or deal them yet
    createDeck();

    // Move the cards to their piles (deal them to their previous places)
    beginDealing();

    highestZ = 1;

    for (int k = 0; k < 4; k++) {
	QString pile;
	pile.sprintf( "FreeCellPile%i", k );
	readPile( cfg, freecellPiles[k], pile, highestZ );
    }

    for (int k = 0; k < 4; k++) {
	QString pile;
	pile.sprintf( "DiscardPile%i", k );
	readPile( cfg, discardPiles[k], pile, highestZ );
    }

    for (int k = 0; k < 8; k++) {
	QString pile;
	pile.sprintf( "WorkingPile%i", k );
	readPile( cfg, workingPiles[k], pile, highestZ );
    }

    highestZ++;

    endDealing();
}


void FreecellCardGame::writeConfig( Config& cfg )
{
    cfg.setGroup("GameState");
    for ( int i = 0; i < 4; i++ ) {
	QString pile;
	pile.sprintf( "FreeCellPile%i", i );
	freecellPiles[i]->writeConfig( cfg, pile );
    }
    for ( int i = 0; i < 4; i++ ) {
	QString pile;
	pile.sprintf( "DiscardPile%i", i );
	discardPiles[i]->writeConfig( cfg, pile );
    }
    for ( int i = 0; i < 8; i++ ) {
	QString pile;
	pile.sprintf( "WorkingPile%i", i );
	workingPiles[i]->writeConfig( cfg, pile );
    }
}


