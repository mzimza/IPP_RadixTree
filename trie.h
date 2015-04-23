/*
 * trie.h
 *
 *  Created on: 13 Apr 2015
 *      Author: Maja Zalewska nr336088
 */

#ifndef TRIE_H_
#define TRIE_H_

//Wstawia podane słowo word do słownika, ewentualnie jego substring od start do end.
int insert(char *, int, int);
//Wstawia do słownika słowo będące podsłowem słowa o numerze number, zaczynającym się na pozycji start i kończącym się na pozycji end (włącznie z tymi dwiema pozycjami). Pozycje liter w słowach numerujemy od 0.
int prev(int, int, int);
//Usuwa ze słownika słowo o numerze number.
int delete (int);
//Daje w wyniku YES albo NO, w zależności od tego, czy któreś ze słów w słowniku ma prefiks równy słowu pattern.
int find (char *);
//Całkowicie czyści słownik
void clear(int *, int *);

#endif /* TRIE_H_ */
