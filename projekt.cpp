#include <iostream>
#include <string.h>
#include <time.h>
#include <windows.h>
using namespace std;

#define MAX_ILOSC_KART 130
#define MAX_DLUGOSC_TEKSTU 1000
#define MAX_LICZBA_KOLOROW 8
#define ILOSC_RUND 3

enum kolorKarty
{
    blue = 0,
    red = 1,
    violet = 2,
    yellow = 3,
    white = 4,
    pink = 5,
    black = 6,
    green = 7
};

struct karta
{
    kolorKarty kolor;
    int wartosc;
};

struct talia
{
    int iloscKolorow;
    int iloscTrucizn;
    int wartoscTrucizny;
    int iloscKartKolor;
    int iloscWszystkichKart;
    karta karty[MAX_ILOSC_KART];
};

struct gracz
{
    int iloscKartGracza = 0;
    int iloscZakrytych = 0;
    karta kartyGracza[MAX_ILOSC_KART];
    karta kartyZakryte[MAX_ILOSC_KART];
    int ilosciKartWKolorach[MAX_LICZBA_KOLOROW] = {0};
    int odpornosc[MAX_LICZBA_KOLOROW] = {0}; // 0 - nieodporny, 1 - odporny
    int punkty = 0;
};

struct kociolek
{
    int iloscKart = 0;
    int suma = 0;
    karta kartyKociolek[MAX_ILOSC_KART];
};

struct kolory
{
    int iloscWystapien = 0;
    int wartosci[MAX_ILOSC_KART];
};

void wyswietlNazweKoloru (kolorKarty kolor, FILE *plik = stdout)
{
    if (kolor == blue)
        fprintf (plik, "blue"); //cout << "blue";
    else if (kolor == red)
        fprintf (plik, "red"); //cout << "red";
    else if (kolor == violet)
        fprintf (plik, "violet"); //cout << "violet";
    else if (kolor == yellow)
        fprintf (plik, "yellow"); //cout << "yellow";
    else if (kolor == white)
        fprintf (plik, "white"); //cout << "white";
    else if (kolor == black)
        fprintf (plik, "black"); //cout << "black";
    else if (kolor == pink)
        fprintf (plik, "pink");
    else
        fprintf (plik, "green"); //cout << "green";
}

kolorKarty zwrocKolor (char *kolor)
{
    if (strcmp(kolor, "blue") == 0)
        return blue;
    else if (strcmp(kolor, "red") == 0)
        return red;
    else if (strcmp(kolor, "violet") == 0)
        return violet;
    else if (strcmp(kolor, "yellow") == 0)
        return yellow;
    else if (strcmp(kolor, "white") == 0)
        return white;
    else if (strcmp(kolor, "black") == 0)
        return black;
    else if (strcmp(kolor, "pink") == 0)
        return pink;
    else
        return green;
}

void usunDodatkowaLinie (char *tekst)
{
    int dlugosc = strlen(tekst);
    if (tekst[dlugosc - 1] == '\n')
        tekst[dlugosc - 1] = '\0';
}

karta *tasujTalie (int iloscKart, karta karty[])
{
    karta *potasowanaTalia = new karta[iloscKart];

    for (int i = 0; i < iloscKart; i++)
    {
        potasowanaTalia[i].wartosc = karty[i].wartosc;
        potasowanaTalia[i].kolor = karty[i].kolor;
    }

    int los1, los2;

    for (int i = 0; i < iloscKart; i++)
    {
        los1 = rand() % iloscKart;
        los2 = rand() % iloscKart;

        kolorKarty pomKolor = potasowanaTalia[los1].kolor;
        int pomWartosc = potasowanaTalia[los1].wartosc;

        potasowanaTalia[los1].kolor = potasowanaTalia[los2].kolor;
        potasowanaTalia[los1].wartosc = potasowanaTalia[los2].wartosc;
        potasowanaTalia[los2].kolor = pomKolor;
        potasowanaTalia[los2].wartosc = pomWartosc;
    }

    return potasowanaTalia;
}

void generujTalie (talia *tal, int wartosci[])
{
    int i;
    for (i = 0; i < tal->iloscTrucizn; i++)
    {
        tal->karty[i].kolor = green;
        tal->karty[i].wartosc = tal->wartoscTrucizny;
    }

    int kolor = blue;
    int ilosc = 0;
    for (; i < tal->iloscWszystkichKart; i++, ilosc++)
    {
        if (ilosc >= tal->iloscKartKolor)
        {
            ilosc = 0;
            kolor++;
        }
        tal->karty[i].kolor = (kolorKarty)kolor;
        tal->karty[i].wartosc = wartosci[ilosc];
    }
}

void wypiszTalie(FILE *plik, karta karty[], int ilosc)
{
    for (int i = 0; i < ilosc; i++)
    {
        fprintf (plik, "%d ", karty[i].wartosc); // cout << karty[i].wartosc << " ";
        wyswietlNazweKoloru(karty[i].kolor, plik);
        fprintf (plik, " "); // cout << " ";
    }
}

void wczytajDane (talia *tal, int **wartosciKart, int *iloscGraczy, gracz **gracze, kociolek **koc, int *iloscKociolkow, int *maxKartKociolek)
{
    cin >> *iloscGraczy >> tal->iloscKolorow >> tal->iloscTrucizn >> tal->wartoscTrucizny >> tal->iloscKartKolor >> *maxKartKociolek;

    tal->iloscWszystkichKart = tal->iloscKolorow * tal->iloscKartKolor + tal->iloscTrucizn;
    *wartosciKart = new int[tal->iloscKartKolor];
    *gracze = new gracz[*iloscGraczy];
    *iloscKociolkow = tal->iloscKolorow;
    *koc = new kociolek[*iloscKociolkow];

    for (int i = 0; i < tal->iloscKartKolor; i++)
        cin >> (*wartosciKart)[i];
}

void rozdajKarty(karta karty[], int iloscKart, gracz gracze[], int iloscGraczy, int rozdajacy)
{
    int nrGracza = rozdajacy; // zaczyna rozdawania od pierwszego gracza po lewej - rozdajacy = index gracza po lewej
    for (int i = 0; i < iloscKart; i++)
    {
        int aktualnieKart = gracze[nrGracza % iloscGraczy].iloscKartGracza;
        gracze[nrGracza % iloscGraczy].kartyGracza[aktualnieKart].kolor = karty[i].kolor;
        gracze[nrGracza % iloscGraczy].kartyGracza[aktualnieKart].wartosc = karty[i].wartosc;
        gracze[nrGracza % iloscGraczy].iloscKartGracza++;
        nrGracza++;
    }
}

void wypiszZawartosciKociolkow (FILE *plik, kociolek koc[], int ilosc)
{
    for (int i = 0; i < ilosc; i++)
    {
        //cout << i + 1 << " pile cards: ";
        fprintf (plik, "%d pile cards: ", i + 1);

        for (int j = 0; j < koc[i].iloscKart; j++)
        {
            //cout << koc[i].kartyKociolek[j].wartosc << " ";
            fprintf (plik, "%d ", koc[i].kartyKociolek[j].wartosc);
            wyswietlNazweKoloru(koc[i].kartyKociolek[j].kolor, plik);
           // cout << " ";
            fprintf (plik, " ");
        }

        //cout << endl;
        fprintf (plik, "\n");
    }
}

void wypiszRozdanie (FILE *plik, gracz gracze[], int iloscGraczy = 3, int aktualnyGracz = 1, int maxKartKociolek = 13)
{
    // wartosci domyslne przedaja sie w poczatkowych zadaniach na STOSie
    //cout << "active player = " << aktualnyGracz << endl;
    //cout << "players number = " << iloscGraczy << endl;
    //cout << "explosion threshold = " << maxKartKociolek << endl;
    fprintf (plik, "active player = %d\n", aktualnyGracz);
    fprintf (plik, "players number = %d\n", iloscGraczy);
    fprintf (plik, "explosion threshold = %d\n", maxKartKociolek);

    for (int i = 0; i < iloscGraczy; i++)
    {
        //cout << i + 1 << " player hand cards: ";
        fprintf (plik, "%d player hand cards: ", i + 1);
        wypiszTalie(plik, gracze[i].kartyGracza, gracze[i].iloscKartGracza);
        //cout << endl << i + 1 << " player deck cards: ";
        fprintf (plik, "\n%d player deck cards: ", i + 1);
        wypiszTalie(plik, gracze[i].kartyZakryte, gracze[i].iloscZakrytych);
        //cout << endl;
        fprintf (plik, "\n");
    }
}

void wczytajStanGry (FILE *plik, int *aktualnyGracz, int *iloscGraczy, gracz **gracze, int *maxKartKociolek)
{
    char tekst[MAX_DLUGOSC_TEKSTU];
    *iloscGraczy = 0;

    // wczytuje 3 pierwsze linie, tj. kolejno active player, players number oraz explosion threshold
    for (int i = 1; i <= 3; i++) // i<= 2, i <= 3 od zadania 6 na STOSIE - dodatkowa linia z inf. o eksplozji kociolka
    {
        //cin.getline(tekst, MAX_DLUGOSC_TEKSTU);
        fgets (tekst, MAX_DLUGOSC_TEKSTU, plik);
        usunDodatkowaLinie(tekst);
        //cout << tekst << endl;
        char *fragment = strtok(tekst, " ");
        while (fragment != NULL)
        {
            if (isdigit(*fragment))
            {
                if (i == 1)
                    *aktualnyGracz = atoi(fragment);
                else if (i == 2)
                    *iloscGraczy = atoi(fragment);
                else
                    *maxKartKociolek = atoi(fragment);
            }

            fragment = strtok(NULL, " ");
        }
    }

    *gracze = new gracz[*iloscGraczy];
}

void generujTalieGracza (char *tekst, karta karty[], int *ilosc, kolory kol[])
{
    int i = 0;
    char *wyraz = strtok(tekst, ":"); // poczatek do znaku dwukropka - n players hand cards:
    wyraz = strtok(NULL, ":"); // tekst, ktory znajduje sie po dwukropku

    wyraz = strtok(wyraz, " "); // wyrazy po spacji
    while (wyraz != NULL)
    {
        int liczba = atoi(wyraz);
        karty[i].wartosc = liczba;

        wyraz = strtok(NULL, " ");
        kolorKarty kolor = zwrocKolor(wyraz);
        karty[i].kolor = kolor;

        kol[kolor].wartosci[kol[kolor].iloscWystapien] = liczba;
        kol[kolor].iloscWystapien++;

        i++;
        wyraz = strtok(NULL, " ");
    }

    *ilosc = i;
}

void wczytajStanGracza (FILE *plik, gracz gracze[], int iloscGraczy, kolory kol[])
{
    char tekst[MAX_DLUGOSC_TEKSTU];

    for (int i = 0; i < iloscGraczy; i++)
    {
        fgets (tekst, MAX_DLUGOSC_TEKSTU, plik);
        //cin.getline(tekst, MAX_DLUGOSC_TEKSTU);
        usunDodatkowaLinie(tekst);
        generujTalieGracza (tekst, gracze[i].kartyGracza, &gracze[i].iloscKartGracza, kol);
        fgets (tekst, MAX_DLUGOSC_TEKSTU, plik);
        //cin.getline(tekst, MAX_DLUGOSC_TEKSTU);
        usunDodatkowaLinie(tekst);
        generujTalieGracza (tekst, gracze[i].kartyZakryte, &gracze[i].iloscZakrytych, kol);
    }
}

void wypiszIlosciKartGraczy (gracz gracze[], int iloscGraczy)
{
    for (int i = 0; i < iloscGraczy; i++)
    {
        cout << i + 1 << " player has " << gracze[i].iloscKartGracza << " cards on hand" << endl;
        //fprintf (plik, "%d player has %d cards on hand\n", i + 1, gracze[i].iloscKartGracza);
        cout << i + 1 << " player has " << gracze[i].iloscZakrytych << " cards in front of him" << endl;
        //fprintf (plik, "%d player has %d cards in front of him\n", i + 1, gracze[i].iloscZakrytych);
    }
}

// zwraca ilosc kolorow zliczonych po wczytaniu stanu gry z pliku
// potrzebna do sprawdzenia, czy w kazdym kolorze jest po tyle samo kart
int zliczIloscKolorow(kolory kol[])
{
    int ilosc = 0;

    for (int i = blue; i < green; i++)
    {
        if (kol[i].iloscWystapien != 0)
            ilosc++;
    }

    return ilosc;
}

void generujKociolek(char *tekst, karta karty[], int *ilosc, kolory kol[])
{
    generujTalieGracza(tekst, karty, ilosc, kol);
}

void wczytajStanKociolka(FILE *plik, kociolek koc[], int iloscKociolkow, kolory kol[])
{
    char tekst[MAX_DLUGOSC_TEKSTU];

    for (int i = 0; i < iloscKociolkow; i++)
    {
        //cin.getline(tekst, MAX_DLUGOSC_TEKSTU);
        fgets (tekst, MAX_DLUGOSC_TEKSTU, plik);
        usunDodatkowaLinie(tekst);
        generujKociolek(tekst, koc[i].kartyKociolek, &koc[i].iloscKart, kol);
    }
}

void wypiszStanyKociolkow (kociolek koc[], int rozmiar)
{
    for (int i = 0; i < rozmiar; i++)
    {
        //fprintf (plik, "there are %d cards on %d pile\n", koc[i].iloscKart, i + 1);
        cout << "there are " << koc[i].iloscKart << " cards on " << i + 1 << " pile" << endl;
        //cout << " suma: " << koc[i].suma << endl;
    }
}

void sprawdzZielone (FILE *plik, kolory kol[])
{
    if (kol[green].iloscWystapien == 0)
        //fprintf (plik, "Green cards does not exist\n");
        cout << "Green cards does not exist" << endl;
    else
    {
        int wartoscZielonych = kol[green].wartosci[0];
        int czyZnalezionoInna = 0;

        for (int i = 1; i < kol[green].iloscWystapien; i++)
        {
            if (kol[green].wartosci[i] != wartoscZielonych)
            {
                czyZnalezionoInna = 1;
                break;
            }
        }

        if (czyZnalezionoInna == 0)
            //fprintf (plik, "Found %d green cards, all with %d value\n", kol[green].iloscWystapien, wartoscZielonych);
            cout << "Found " << kol[green].iloscWystapien << " green cards, all with " << wartoscZielonych << " value" << endl;
        else
            //fprintf (plik, "Different green cards values occurred\n");
            cout << "Different green cards values occurred" << endl;
    }
}

// zwraca ilosc kolorow, jesli wszystkie sie zgadzaja, 0 w przeciwnym wypadku
int sprawdzNieZielone (kolory kol[])
{
    int czyZnalezionoInna = 0;
    int ilosc = 0;
    for (int i = blue; i < green; i++)
    {
        if (kol[i].iloscWystapien != 0) // jesli dany kolor wystepuje w rozgyrwce
        {
            ilosc = kol[i].iloscWystapien;

            for (int j = i + 1; j < green; j++) // to ilosci kart w tym kolorze i w kazdym innym, ktory wystepuje powinny byc takie same
            {
                if (kol[j].iloscWystapien != 0 && kol[j].iloscWystapien != ilosc)
                {
                    czyZnalezionoInna = 1;
                    break;
                }
            }

            break;
        }
    }

    if (czyZnalezionoInna == 0)
        return ilosc;

    return 0;
}

void wypiszKomunikatOIlosciKart (int wynikSprawdzenia, kolory kol[])
{
    if (wynikSprawdzenia != 0)
        //fprintf (plik, "The number cards of all colors is equal: %d\n", wynikSprawdzenia);
        cout << "The number cards of all colors is equal: " << wynikSprawdzenia << endl;
    else
    {
        //fprintf (plik, "At least two colors with a different number of cards were found: \n");
        cout << "At least two colors with a different number of cards were found: " << endl;

        for (int i = blue; i < green; i++)
        {
            wyswietlNazweKoloru((kolorKarty)i);
            //fprintf (plik, " cards are: %d\n", kol[i].iloscWystapien);
            cout << " cards are " << kol[i].iloscWystapien << endl;
        }
    }
}

int porownaj (const void *liczba1, const void *liczba2)
{
    int wartosc1, wartosc2;
    wartosc1 = *(int*) liczba1; // (int*)liczba1 - rzutowanie oryginalnego wskaznika na wskaznik *int
    wartosc2 = *(int*) liczba2; // *(int*)liczba1 - odczytanie wartosci rzutowanego wskaznika

    if (wartosc1 > wartosc2)
        return 1;
    if (wartosc1 < wartosc2)
        return -1;

    return 0;
}

void sortujWartosci (kolory kol[])
{
    for (int i = 0; i < MAX_LICZBA_KOLOROW; i++)
    {
        qsort(kol[i].wartosci, kol[i].iloscWystapien, sizeof(int), porownaj);
        // wskaznik do tablicy, liczba elementow, rozmiar w bajtach kazdego elementu, funkcja porownujaca
    }
}

// sprawdza czy wartosci kart w kazdym z kolorow sa takie same
// jesli tak, to zwraca 1, w przeciwnym wypadku zwraca 0
int sprawdzWartosci (kolory kol[])
{
    int czyZnalezionoInne = 0;

    if (sprawdzNieZielone(kol) != 0)
    {
        for (int i = blue; i < green; i++)
        {
            if (kol[i].iloscWystapien != 0)
            {
                for (int j = blue + 1; j < green; j++)
                {
                    if ((kol[j].iloscWystapien != 0) && (memcmp(kol[i].wartosci, kol[j].wartosci, kol[i].iloscWystapien*sizeof(int)) != 0))
                    {
                        czyZnalezionoInne = 1; // memcmp porownuje dwa bloki pamieci (dwie tablice) i zwraca 0 jesli sa rowne
                        break;
                    }
                }
            }
        }

        if (czyZnalezionoInne == 0)
            return 1;
        else
            return 0;
    }
    else
        return 0;
}

void wypiszWartosci (kolory kol[])
{
    for (int i = blue; i < green; i++)
    {
        if (kol[i].iloscWystapien != 0)
        {
            wyswietlNazweKoloru((kolorKarty)i);
            //fprintf (plik, " cards values: ");
            cout << " cards values: ";

            for (int j = 0; j < kol[i].iloscWystapien; j++)
                //fprintf (plik, "%d ", kol[i].wartosci[j]);
                cout << kol[i].wartosci[j] << " ";

            cout << endl;
        }

        //fprintf (plik, "\n");
    }
}

void wypiszKomunikatOWartosciachKart (int wynikSprawdzenia, kolory kol[])
{
    if (wynikSprawdzenia == 1)
    {
        //fprintf (plik, "The values of cards of all colors are identical:\n");
        cout << "The values of cards of all colors are identical:" << endl;

        for (int i = 0; i < MAX_LICZBA_KOLOROW; i++)
        {
            if (kol[i].iloscWystapien != 0)
            {
                for (int j = 0; j < kol[i].iloscWystapien; j++)
                    //fprintf (plik, "%d ", kol[i].wartosci[j]);
                    cout << kol[i].wartosci[j] << " ";

                break;
            }
        }

        cout << endl;
    }

    else
    {
        //fprintf (plik, "The values of cards of all colors are not identical:\n");
        cout << "The values of cards of all colors are not identical:" << endl;
        wypiszWartosci(kol);
    }
}

// sprawdza czy liczba kart na rece kazdego z graczy jest prawidlowa
// kazdy z graczy powinien miec taka sama ilosc kart jak aktualny gracz, jesli nie wykonal on jeszcze ruchu
// lub o jedna karte wiecej, jesli aktualny gracz wykonal juz ruch
int sprawdzLiczbeKart (gracz gracze[], int iloscGraczy, int aktualnyGracz)
{
    int iloscKart = gracze[aktualnyGracz-1].iloscKartGracza;
    int czyBlad = 0;

    for (int i = 0; i < iloscGraczy; i++)
    {
        if (gracze[i].iloscKartGracza != iloscKart && gracze[i].iloscKartGracza != iloscKart - 1)
        {
            czyBlad = 1;
            break;
        }
    }

    if (czyBlad == 0)
        return 1;

    return 0;

}

void obliczSumyWKociolkach (kociolek koc[], int iloscKociolkow)
{
    for (int i = 0; i < iloscKociolkow; i++)
    {
        for (int j = 0; j < koc[i].iloscKart; j++)
            koc[i].suma += koc[i].kartyKociolek[j].wartosc;
    }
}

// zwraca numer koloru kociolka, jesli zawiera takie same kolory
// -1 w przeciwnym razie
// jesli kociolek jest pusty, to zwraca kolor green
int sprawdzKoloryKociolkow (karta karty[], int iloscKart)
{
    int czyRozneKolory = 0;
    kolorKarty kolor = green;

    for (int i = 0; i < iloscKart; i++)
    {
        if (karty[i].kolor != green)
        {
            kolor = karty[i].kolor;
            break;
        }
    }

    for (int i = 0; i < iloscKart; i++)
    {
        if (karty[i].kolor != green && karty[i].kolor != kolor)
        {
            czyRozneKolory = 1;
            break;
        }
    }

    if (czyRozneKolory == 1)
        return -1;

    return kolor;
}

int sprawdzSumyWKociolkach (kociolek koc[], int iloscKociolkow, int maxSumaKociolek)
{
    int czyWszystkoOk = 1;

    for (int i = 0; i < iloscKociolkow; i++)
    {
        if (koc[i].suma > maxSumaKociolek)
        {
            //fprintf (plik, "Pile number %d should explode earlier\n", i + 1);
            cout << "Pile number " << i + 1 << " should explode earlier" << endl;
            czyWszystkoOk = 0;
        }
    }

    if (czyWszystkoOk == 1)
        return 1;

    return 0;
}

void sprawdzPoprawnoscStanuGry (gracz gracze[], int iloscGraczy, int aktualnyGracz, kociolek koc[], int iloscKociolkow, int maxKartKociolek)
{
    int wszystkoOk = 1;

    if (sprawdzLiczbeKart(gracze, iloscGraczy, aktualnyGracz) == 0)
    {
        //fprintf (plik, "The number of players cards on hand is wrong\n");
        cout << "The number of players cards on hand is wrong" << endl;
        wszystkoOk = 0;
    }

    for (int i = 0; i < iloscKociolkow; i++)
    {
        if (sprawdzKoloryKociolkow(koc[i].kartyKociolek , koc[i].iloscKart) == -1)
        {
            //fprintf (plik, "Two different colors were found on the %d pile\n", i + 1);
            cout << "Two different colors were found on the " << i + 1 << " pile" << endl;
            wszystkoOk = 0;
        }
    }

    if (sprawdzSumyWKociolkach(koc, iloscKociolkow, maxKartKociolek) == 0)
        wszystkoOk = 0;

    if (wszystkoOk == 1)
    {
        cout << "Current state of the game is ok" << endl;
        //fprintf (plik, "Current state of the game is ok\n");
    }
}

void aktualizujTalie (karta karty[], int *iloscKart, karta k)
{
    int index = -1;
    // szuka pozycji (indeksu w tablicy) karty, ktora odlozyl gracz
    for (int i = 0; i < *iloscKart; i++)
    {
        if (karty[i].kolor == k.kolor && karty[i].wartosc == k.wartosc)
        {
            index = i;
            break;
        }
    }

    // "usuwa" karte z talii gracza, przesuwajac wszystkie karty stojace od niej na prawo o jedna pozycje w lewo
    for (int i = index; i < *iloscKart - 1; i++)
    {
        karty[i].kolor = karty[i + 1].kolor;
        karty[i].wartosc = karty[i + 1].wartosc;
    }

    (*iloscKart)--;
}

void aktualizujKociolek (kociolek koc[], int numer, karta k)
{
    koc[numer].kartyKociolek[koc[numer].iloscKart].kolor = k.kolor;
    koc[numer].kartyKociolek[koc[numer].iloscKart].wartosc = k.wartosc;

    koc[numer].iloscKart++;
    koc[numer].suma += k.wartosc;
}

void eksplozjaKociolka(kociolek koc[], int nrKociolka, karta kartyZakryte[], int *iloscKartZakrytych)
{
    for (int i = 0; i < koc[nrKociolka].iloscKart; i++)
    {
        kartyZakryte[*iloscKartZakrytych].kolor = koc[nrKociolka].kartyKociolek[i].kolor;
        kartyZakryte[*iloscKartZakrytych].wartosc = koc[nrKociolka].kartyKociolek[i].wartosc;
        (*iloscKartZakrytych)++;
    }

    koc[nrKociolka].iloscKart = 0;
    koc[nrKociolka].suma = 0;
}

void wykonajPosuniecie(karta k, kociolek koc[], int nrKociolka, gracz gracze[], int iloscGraczy, int *aktualnyGracz,
                       int maxKartKociolek)
{
    int indexGracza = *aktualnyGracz - 1;

    aktualizujTalie(gracze[indexGracza].kartyGracza, &(gracze[indexGracza].iloscKartGracza), k);
    aktualizujKociolek(koc, nrKociolka, k);

    if (koc[nrKociolka].suma > maxKartKociolek)
        eksplozjaKociolka(koc, nrKociolka, gracze[indexGracza].kartyZakryte, &(gracze[indexGracza].iloscZakrytych));

    (*aktualnyGracz)++;
    if (*aktualnyGracz > iloscGraczy)
        (*aktualnyGracz) %= iloscGraczy;

}

int wybierzKociolekONajmniejszejIlosciKart (kolorKarty kol, kociolek koc[], int iloscKociolkow)
{
    int minIlosc = -1, nrKociolka;

    for (int i = 0; i < iloscKociolkow; i++)
    {
        // jesli karta ma kolor green to mozna dla niej wybrac dowolny najbardziej pusty kociolek
        // w przeciwnym razie trzeba ja odlozyc na kociolek o takim samym kolorze (jesli istnieje)
        if ((kol != green && sprawdzKoloryKociolkow(koc[i].kartyKociolek, koc[i].iloscKart) == kol) || (kol == green))
        {
            if (minIlosc == -1)
            {
                minIlosc = koc[i].iloscKart;
                nrKociolka = i;
            }

            else
            {
                if (koc[i].iloscKart < minIlosc)
                {
                    minIlosc = koc[i].iloscKart;
                    nrKociolka = i;
                }
            }
        }
    }

    // jesli minIlosc niezdefiniowana, tzn., ze nie ma kociolka w takim kolorze, wiec wybieramy pusty(zielony)
    if (minIlosc == -1)
    {
        for (int i = 0; i < iloscKociolkow; i++)
        {
            if (sprawdzKoloryKociolkow(koc[i].kartyKociolek, koc[i].iloscKart) == green)
            {
                if (minIlosc == -1)
                {
                    minIlosc = koc[i].iloscKart;
                    nrKociolka = i;
                }

                else
                {
                    if (koc[i].iloscKart < minIlosc)
                    {
                        minIlosc = koc[i].iloscKart;
                        nrKociolka = i;
                    }
                }
            }
        }
    }

    return nrKociolka;
}

// zwraca nr kociolka o najmniejszej sumie wartosci kart
int wybierzKociolek (karta k, kociolek koc[], int iloscKociolkow)
{
    int numer = 0;
    kolorKarty kolorKociolka = green;

    // jesli karta zielona, to mozna odlozyc na dowolny kociolek o najmniejszej sumie kart
    if (k.kolor == green)
    {
        int minSuma = koc[0].suma;

        for (int i = 0; i < iloscKociolkow; i++)
        {
            if (koc[i].suma < minSuma)
            {
                numer = i;
                minSuma = koc[i].suma;
            }
        }
    }

    else
    {
        int znaleziono = 0;

        // szuka pierwszego dozwolonego kociolka o takim samym kolorze jak karta
        for (int i = 0; i < iloscKociolkow; i++)
        {
            if (sprawdzKoloryKociolkow(koc[i].kartyKociolek, koc[i].iloscKart) == k.kolor)
            {
                numer = i;
                kolorKociolka = k.kolor;
                znaleziono = 1;
                break;
            }
        }

        // jesli nie znajdzie kociolka w tym kolorze, to szuka zielonego lub pustego
        if (znaleziono == 0)
        {
            for (int i = 0; i < iloscKociolkow; i++)
            {
                if (sprawdzKoloryKociolkow(koc[i].kartyKociolek, koc[i].iloscKart) == green)
                {
                    numer = i;
                    znaleziono = 1;
                    break;
                }
            }
        }

        // sprawdza czy istnieje bardziej optymalny kociolek od aktualnie wybranego
        for (int i = 0; i < iloscKociolkow; i++)
        {
            if (sprawdzKoloryKociolkow(koc[i].kartyKociolek, koc[i].iloscKart) == kolorKociolka)
            {
                if (koc[i].suma < koc[numer].suma)
                    numer = i;
            }
        }
    }

    return numer;
}

// zwraca maksymalna ilosc kart danego koloru, jaka gracz moze uzyskac w wyniku eksplozji kociolka
// przydatne do maksymalizacji koloru
int wyznaczMaxIloscKart (kolorKarty kol, karta kartyKociolek[], int iloscKart, gracz gr)
{
    for (int i = 0; i < gr.iloscZakrytych; i++)
    {
        if (gr.kartyZakryte[i].kolor == kol)
            gr.ilosciKartWKolorach[kol]++;
    }

    for (int i = 0; i < iloscKart; i++)
        gr.ilosciKartWKolorach[kartyKociolek[i].kolor]++;

    int maxIlosc = gr.ilosciKartWKolorach[blue];

    for (int i = blue; i <= green; i++)
    {
        if (gr.ilosciKartWKolorach[i] > maxIlosc && i != green)
            maxIlosc = gr.ilosciKartWKolorach[i];
    }

    return maxIlosc;
}

void maksymalizujKolor (gracz gracze[], int iloscGraczy, int *aktualnyGracz, kociolek koc[], int iloscKociolkow, int maxKartKociolek)
{
    int indexGracza = *aktualnyGracz - 1;
    karta k, optymalnaKarta;
    int pomMaxKart, maxKart, maxSuma, optymalnyKociolek, znalezionoMax = 0, znalezionoKociolek;

    // wybiera karte, a nasteptnie szuka dla niej dozwolonego kociolka
    for (int i = 0; i < gracze[indexGracza].iloscKartGracza; i++)
    {
        k.wartosc = gracze[indexGracza].kartyGracza[i].wartosc;
        k.kolor = gracze[indexGracza].kartyGracza[i].kolor;

        for (int j = 0; j < iloscKociolkow; j++)
        {
            znalezionoKociolek = 0;
            kolorKarty kolorKociolka = kolorKarty(sprawdzKoloryKociolkow(koc[j].kartyKociolek, koc[j].iloscKart));

            if (k.kolor == kolorKociolka || k.kolor == green)
            {
                znalezionoKociolek = 1;
                // sprawdza ile kart danego koloru mialby gracz w wyniku potencjalnej eksplozji
                pomMaxKart = wyznaczMaxIloscKart (kolorKociolka, koc[j].kartyKociolek, koc[j].iloscKart, gracze[indexGracza]);

                if (k.kolor != green)
                    pomMaxKart++;

                // jesli wczesniej nie zostala znaleziona max ilosc kart danego koloru w wyniku eksplozji
                // to aktualna jest najlepsza
                if (znalezionoMax == 0)
                {
                    maxKart = pomMaxKart;
                    optymalnaKarta.wartosc = k.wartosc;
                    optymalnaKarta.kolor = k.kolor;
                    optymalnyKociolek = j;
                    maxSuma = koc[j].suma + k.wartosc;
                    znalezionoMax = 1;
                }

                else
                {
                    // spr, czy w wyniku eksplozji po dolozeniu danej karty gracz bedzie mial wieksza ilosc kart danego koloru
                    // jesli ilosc bedzie taka sama, ale dolozenie aktualnej karty szybciej przyczyni sie do eksplozji (wieksza suma)
                    // to wybieramy wlasnie te karte
                    if ((pomMaxKart > maxKart) || (pomMaxKart == maxKart && koc[j].suma + k.wartosc > maxSuma))
                    {
                        maxKart = pomMaxKart;
                        optymalnaKarta.wartosc = k.wartosc;
                        optymalnaKarta.kolor = k.kolor;
                        optymalnyKociolek = j;
                        maxSuma = koc[j].suma + k.wartosc;
                    }
                }

                if (k.kolor != green)
                    break;
            }
        }

        if (znalezionoKociolek == 0) // jesli nie znaleziono
        {
            for (int j = 0; j < iloscKociolkow; j++)
            {
                kolorKarty kolorKociolka = kolorKarty(sprawdzKoloryKociolkow(koc[j].kartyKociolek, koc[j].iloscKart));

                if (kolorKociolka == green)
                {
                    znalezionoKociolek = 1;
                    pomMaxKart = wyznaczMaxIloscKart (k.kolor, koc[j].kartyKociolek, koc[j].iloscKart, gracze[indexGracza]);
                    pomMaxKart++;

                    if (znalezionoMax == 0)
                    {
                        maxKart = pomMaxKart;
                        optymalnaKarta.wartosc = k.wartosc;
                        optymalnaKarta.kolor = k.kolor;
                        optymalnyKociolek = j;
                        maxSuma = koc[j].suma + k.wartosc;
                        znalezionoMax = 1;
                    }

                    else
                    {
                        if ((pomMaxKart > maxKart) || (pomMaxKart == maxKart && koc[j].suma + k.wartosc > maxSuma))
                        {
                            maxKart = pomMaxKart;
                            optymalnaKarta.wartosc = k.wartosc;
                            optymalnaKarta.kolor = k.kolor;
                            optymalnyKociolek = j;
                            maxSuma = koc[j].suma + k.wartosc;
                        }
                    }

                    break;
                }
            }
        }
    }

    wykonajPosuniecie(optymalnaKarta, koc, optymalnyKociolek, gracze, iloscGraczy, aktualnyGracz, maxKartKociolek);
}

void wybierzNajwiekszaKarte (gracz gracze[], int iloscGraczy, int *aktualnyGracz, kociolek koc[], int iloscKociolkow, int maxKartKociolek)
{
    int indexGracza = *aktualnyGracz - 1;
    karta maxKarta, pomKarta;
    maxKarta.wartosc = gracze[indexGracza].kartyGracza[0].wartosc;
    maxKarta.kolor = gracze[indexGracza].kartyGracza[0].kolor;
    int maxWartosc = gracze[indexGracza].kartyGracza[0].wartosc;
    int nrKociolka = wybierzKociolekONajmniejszejIlosciKart(maxKarta.kolor, koc, iloscKociolkow), pomNrKociolka;

    for (int i = 0; i < gracze[indexGracza].iloscKartGracza; i++)
    {
        if (gracze[indexGracza].kartyGracza[i].wartosc > maxWartosc)
        {
            maxWartosc = gracze[indexGracza].kartyGracza[i].wartosc;
            maxKarta.wartosc = gracze[indexGracza].kartyGracza[i].wartosc;
            maxKarta.kolor = gracze[indexGracza].kartyGracza[i].kolor;
            //nrKociolka = wybierzKociolek(maxKarta, koc, iloscKociolkow);
            nrKociolka = wybierzKociolekONajmniejszejIlosciKart(maxKarta.kolor, koc, iloscKociolkow);
        }

        else if (gracze[indexGracza].kartyGracza[i].wartosc == maxWartosc)
        {
            pomKarta.wartosc = gracze[indexGracza].kartyGracza[i].wartosc;
            pomKarta.kolor = gracze[indexGracza].kartyGracza[i].kolor;
            //pomNrKociolka = wybierzKociolek(pomKarta, koc, iloscKociolkow);
            pomNrKociolka = wybierzKociolekONajmniejszejIlosciKart(pomKarta.kolor, koc, iloscKociolkow);

            // jesli jest kilka najwiekszych kart, to wybieramy te, ktora mozna wrzucic do kociolka o najmniejszej ilosci kart
            if (koc[pomNrKociolka].iloscKart < koc[nrKociolka].iloscKart)
            {
                maxKarta.wartosc = pomKarta.wartosc;
                maxKarta.kolor = pomKarta.kolor;
                nrKociolka = pomNrKociolka;
            }
        }
    }

    wykonajPosuniecie(maxKarta, koc, nrKociolka, gracze, iloscGraczy, aktualnyGracz, maxKartKociolek);
}

void wybierzNajmniejszaKarte (gracz gracze[], int iloscGraczy, int *aktualnyGracz, kociolek koc[], int iloscKociolkow, int maxKartKociolek)
{
    int indexGracza = *aktualnyGracz - 1;
    karta minKarta, pomKarta;
    minKarta.wartosc = gracze[indexGracza].kartyGracza[0].wartosc;
    minKarta.kolor = gracze[indexGracza].kartyGracza[0].kolor;
    int minWartosc = gracze[indexGracza].kartyGracza[0].wartosc;
    int nrKociolka = wybierzKociolek(minKarta, koc, iloscKociolkow), pomNrKociolka;

    // szuka najmniejszej karty oraz kociolka o najmniejszej sumie wartosci kart
    for (int i = 0; i < gracze[indexGracza].iloscKartGracza; i++)
    {
        if (gracze[indexGracza].kartyGracza[i].wartosc < minWartosc)
        {
            minWartosc = gracze[indexGracza].kartyGracza[i].wartosc;
            minKarta.wartosc = gracze[indexGracza].kartyGracza[i].wartosc;
            minKarta.kolor = gracze[indexGracza].kartyGracza[i].kolor;
            nrKociolka = wybierzKociolek(minKarta, koc, iloscKociolkow);
        }

        // jesli znajdzie kilka najmniejszych kart, to wybiera te, dla ktorej kociolek ma mniejsza sume
        else if (gracze[indexGracza].kartyGracza[i].wartosc == minWartosc)
        {
            pomKarta.wartosc = gracze[indexGracza].kartyGracza[i].wartosc;
            pomKarta.kolor = gracze[indexGracza].kartyGracza[i].kolor;
            pomNrKociolka = wybierzKociolek(pomKarta, koc, iloscKociolkow);

            if (koc[pomNrKociolka].suma < koc[nrKociolka].suma)
            {
                minKarta.wartosc = pomKarta.wartosc;
                minKarta.kolor = pomKarta.kolor;
                nrKociolka = pomNrKociolka;
            }
        }
    }

    if (koc[nrKociolka].suma + minKarta.wartosc > maxKartKociolek)
        wybierzNajwiekszaKarte(gracze, iloscGraczy, aktualnyGracz, koc, iloscKociolkow, maxKartKociolek);

    else
       wykonajPosuniecie(minKarta, koc, nrKociolka, gracze, iloscGraczy, aktualnyGracz, maxKartKociolek);
}

// wybiera jak najwieksza karte, ktora nie spowoduje eksplozji
// jesli to niemozliwe, to pozbywa sie najwiekszej
void wybierzOptymalnaKarte(gracz gracze[], int iloscGraczy, int *aktualnyGracz, kociolek koc[], int iloscKociolkow, int maxKartKociolek)
{
    int indexGracza = *aktualnyGracz - 1;
    karta optymalnaKarta, pomKarta;
    int maxWartosc, nrKociolka, pomNrKociolka, znaleziono = 0;

    for (int i = 0; i < gracze[indexGracza].iloscKartGracza; i++)
    {
        if (znaleziono == 0)
        {
            pomKarta.wartosc = gracze[indexGracza].kartyGracza[i].wartosc;
            pomKarta.kolor = gracze[indexGracza].kartyGracza[i].kolor;
            pomNrKociolka = wybierzKociolek(pomKarta, koc, iloscKociolkow);

            if (koc[pomNrKociolka].suma + pomKarta.wartosc <= maxKartKociolek)
            {
                znaleziono = 1;
                optymalnaKarta.wartosc = pomKarta.wartosc;
                optymalnaKarta.kolor = pomKarta.kolor;
                nrKociolka = pomNrKociolka;
                maxWartosc = optymalnaKarta.wartosc;
            }
        }

        else if (gracze[indexGracza].kartyGracza[i].wartosc > maxWartosc)
        {
            pomKarta.wartosc = gracze[indexGracza].kartyGracza[i].wartosc;
            pomKarta.kolor = gracze[indexGracza].kartyGracza[i].kolor;
            pomNrKociolka = wybierzKociolek(pomKarta, koc, iloscKociolkow);

            if (koc[pomNrKociolka].suma + pomKarta.wartosc <= maxKartKociolek)
            {
                optymalnaKarta.wartosc = pomKarta.wartosc;
                optymalnaKarta.kolor = pomKarta.kolor;
                nrKociolka = pomNrKociolka;
                maxWartosc = optymalnaKarta.wartosc;
            }
        }
    }

    if (znaleziono == 0)
        wybierzNajwiekszaKarte(gracze, iloscGraczy, aktualnyGracz, koc, iloscKociolkow, maxKartKociolek);

    else
        wykonajPosuniecie(optymalnaKarta, koc, nrKociolka, gracze, iloscGraczy, aktualnyGracz, maxKartKociolek);
}

// wybiera pierwsza karte i odklada ja do pierwszej dozwolonego kociolka
void prostePosuniecie (gracz gracze[], int iloscGraczy, int *aktualnyGracz, kociolek koc[], int iloscKociolkow, int maxKartKociolek)
{
    int wykonano = 0;
    int indexGracza = *aktualnyGracz - 1;

    if (gracze[indexGracza].iloscKartGracza > 0)
    {
        karta k;
        k.wartosc = gracze[indexGracza].kartyGracza[0].wartosc;
        k.kolor = gracze[indexGracza].kartyGracza[0].kolor;

        if (k.kolor == green)
        {
            wykonajPosuniecie(k, koc, 0, gracze, iloscGraczy, aktualnyGracz, maxKartKociolek);
            wykonano = 1;
        }

        else
        {
            for (int i = 0; i < iloscKociolkow; i++)
            {
                if (sprawdzKoloryKociolkow(koc[i].kartyKociolek, koc[i].iloscKart) == k.kolor)
                {
                    wykonajPosuniecie(k, koc, i, gracze, iloscGraczy, aktualnyGracz, maxKartKociolek);
                    wykonano = 1;
                    break;
                }
            }

            if (wykonano == 0) // jesli nie znaleziono kociolka z danym kolorem, to szuka kociolka pustego lub z zielonym
            {
                for (int i = 0; i < iloscKociolkow; i++)
                {
                    if (sprawdzKoloryKociolkow(koc[i].kartyKociolek, koc[i].iloscKart) == green)
                    {
                        wykonajPosuniecie(k, koc, i, gracze, iloscGraczy, aktualnyGracz, maxKartKociolek);
                        wykonano = 1;
                        break;
                    }
                }
            }
        }
    }
}

void podliczIlosciKart(gracz gracze[], int iloscGraczy)
{
    for (int i = 0; i < iloscGraczy; i++)
    {
        for (int j = blue; j <= green; j++)
        {
            for (int k = 0; k < gracze[i].iloscZakrytych; k++)
            {
                if (gracze[i].kartyZakryte[k].kolor == j)
                    gracze[i].ilosciKartWKolorach[j]++;
            }
        }
    }
}

void sprawdzOdpornosc (gracz gracze[], int iloscGraczy)
{
    for (int i = blue; i < green; i++)
    {
        for (int j = 0; j < iloscGraczy; j++)
        {
            int ilosc = gracze[j].ilosciKartWKolorach[i];
            int czyOdporny = 1;

            for (int k = 0; k < iloscGraczy; k++)
            {
                if (k != j)
                {
                    if (gracze[k].ilosciKartWKolorach[i] >= ilosc)
                    {
                        czyOdporny = 0;
                        break;
                    }
                }
            }

            if (czyOdporny == 1)
            {
                //fprintf (plik, "Na kolor: ");
                cout << "Na kolor ";
                wyswietlNazweKoloru((kolorKarty)i);
                //fprintf (plik, " odporny jest gracz: %d\n", j + 1);
                cout << " odporny jest gracz " << j + 1 << endl;
                gracze[j].odpornosc[i] = 1;
                break;
            }
        }
    }
}

void podliczPunkty (gracz gracze[], int iloscGraczy)
{
    for (int i = 0; i < iloscGraczy; i++)
    {
        for (int j = 0; j < gracze[i].iloscZakrytych; j++)
        {
            if (gracze[i].kartyZakryte[j].kolor == green)
                gracze[i].punkty += 2;
            else if (gracze[i].odpornosc[gracze[i].kartyZakryte[j].kolor] == 0)
                gracze[i].punkty += 1;
        }
    }
}

void wypiszPunkty (gracz gracze[], int iloscGraczy)
{
    for (int i = 0; i < iloscGraczy; i++)
        //fprintf (plik, "Wynik gracza %d = %d\n", i + 1, gracze[i].punkty);
        cout << "Wynik gracza " << i + 1 << " = " << gracze[i].punkty << endl;
}

void wyzerujTablice (gracz gracze[], int iloscGraczy, kociolek koc[], int iloscKociolkow)
{
    for (int i = 0; i < iloscGraczy; i++)
    {
        gracze[i].iloscKartGracza = 0;
        gracze[i].iloscZakrytych = 0;
        gracze[i].ilosciKartWKolorach[MAX_LICZBA_KOLOROW] = {0};
        gracze[i].odpornosc[MAX_LICZBA_KOLOROW] = {0};
        gracze[i].punkty = 0;
    }

    for (int i = 0; i < iloscKociolkow; i++)
    {
        koc[i].iloscKart = 0;
        koc[i].suma = 0;
    }
}

void podliczIloscWygranych (gracz gracze[], int iloscGraczy, int iloscWygranych[])
{
    int indexZwyciezcy = 0;

    for (int i = 1; i < iloscGraczy; i++) // spr od i = 1, bo 0 domylsnie
    {
        if (gracze[i].punkty < gracze[indexZwyciezcy].punkty)
            indexZwyciezcy = i;
    }

    // spr czy ktorys gracz zremisowal ze zwyciezca - jesli tak, to wygrali obaj
    for (int i = 0; i < iloscGraczy; i++)
    {
        if (gracze[i].punkty == gracze[indexZwyciezcy].punkty && i != indexZwyciezcy)
            iloscWygranych[i]++;
    }

    iloscWygranych[indexZwyciezcy]++;
}

void koniecRundy(gracz gracze[], int iloscGraczy, kociolek koc[], int iloscKociolkow, int iloscWygranych[])
{
    podliczIlosciKart(gracze, iloscGraczy);
    sprawdzOdpornosc(gracze, iloscGraczy);
    podliczPunkty (gracze, iloscGraczy);
    podliczIloscWygranych (gracze, iloscGraczy, iloscWygranych);
    wypiszPunkty (gracze, iloscGraczy);
    wyzerujTablice (gracze, iloscGraczy, koc, iloscKociolkow);
}

int sprawdzCzyKoniecRundy (gracz gracze[], int iloscGraczy)
{
    // koniec rundy nastepuje jesli talie graczy sa puste
    int czyPuste = 1;
    for (int i = 0; i < iloscGraczy; i++)
    {
        if (gracze[i].iloscKartGracza != 0)
        {
            czyPuste = 0;
            break;
        }
    }

    if (czyPuste == 1)
        return 1;

    return 0;
}

void wypiszStatystykiGraczy (int iloscWygranych[], int iloscGraczy)
{
    FILE *stat;
    stat = fopen("statystyki.txt", "w");

    fprintf (stat, "Statystyki graczy dla %d rund\n", ILOSC_RUND);

    for (int i = 0; i < iloscGraczy; i++)
        fprintf (stat, "Gracz nr %d wygral: %d razy\n", i + 1, iloscWygranych[i]);

    fclose (stat);
}

int main()
{
    srand (time(NULL));
    FILE *plikTalia;
    plikTalia = fopen ("talia.txt", "w");

    talia tal;
    gracz *gracze;
    kociolek *koc;
    int *wartosciKart, *iloscWygranych;
    int iloscGraczy, aktualnyGracz = 1, iloscKociolkow, maxKartKociolek, wynikSprawdzenia, runda = 1, rozdajacy = 1;

    wczytajDane (&tal, &wartosciKart, &iloscGraczy, &gracze, &koc, &iloscKociolkow, &maxKartKociolek);
    generujTalie (&tal, wartosciKart);
    wypiszTalie(plikTalia, tal.karty, tal.iloscWszystkichKart);
    fclose (plikTalia);

    iloscWygranych = new int[iloscGraczy];
    for (int i = 0; i < iloscGraczy; i++)
        iloscWygranych[i] = 0;

    while (runda <= ILOSC_RUND)
    {
        FILE *plik;
        plik = fopen("stan.txt", "w");

        if (rozdajacy > iloscGraczy)
            rozdajacy %= iloscGraczy;

        aktualnyGracz = rozdajacy + 1;
        if (aktualnyGracz > iloscGraczy)
            aktualnyGracz %= iloscGraczy;

        karta *potasowanaTalia;
        potasowanaTalia = tasujTalie(tal.iloscWszystkichKart, tal.karty);
        rozdajKarty (potasowanaTalia, tal.iloscWszystkichKart, gracze, iloscGraczy, rozdajacy);
        wypiszRozdanie (plik, gracze, iloscGraczy, aktualnyGracz, maxKartKociolek);
        wypiszZawartosciKociolkow (plik, koc, iloscKociolkow);
        fclose (plik);

        while (sprawdzCzyKoniecRundy(gracze, iloscGraczy) == 0)
        {
            kolory kol[MAX_LICZBA_KOLOROW];
            plik = fopen("stan.txt", "r");
            wczytajStanGry(plik, &aktualnyGracz, &iloscGraczy, &gracze, &maxKartKociolek);
            wczytajStanGracza(plik, gracze, iloscGraczy, kol);

            iloscKociolkow = zliczIloscKolorow(kol);
            koc = new kociolek[iloscKociolkow];
            wczytajStanKociolka(plik, koc, iloscKociolkow, kol);
            obliczSumyWKociolkach (koc, iloscKociolkow);
            fclose (plik);

            wypiszIlosciKartGraczy(gracze, iloscGraczy);
            wypiszStanyKociolkow(koc, iloscKociolkow);

            sprawdzZielone (plik, kol);
            wynikSprawdzenia = sprawdzNieZielone(kol);
            wypiszKomunikatOIlosciKart(wynikSprawdzenia, kol);

            sortujWartosci(kol);
            wynikSprawdzenia = sprawdzWartosci(kol);
            wypiszKomunikatOWartosciachKart(wynikSprawdzenia, kol);

            cout << endl << "Oczekiwanie na ruch gracza nr " << aktualnyGracz << "..." << endl << endl;
            Sleep (500);

            plik = fopen("stan.txt", "w");
            if (aktualnyGracz == 1)
                wybierzNajmniejszaKarte(gracze, iloscGraczy, &aktualnyGracz, koc, iloscKociolkow, maxKartKociolek);
            else if (aktualnyGracz == 2)
                wybierzOptymalnaKarte(gracze, iloscGraczy, &aktualnyGracz, koc, iloscKociolkow, maxKartKociolek);
            else if (aktualnyGracz == 3)
                wybierzNajwiekszaKarte(gracze, iloscGraczy, &aktualnyGracz, koc, iloscKociolkow, maxKartKociolek);
            else
                prostePosuniecie(gracze, iloscGraczy, &aktualnyGracz, koc, iloscKociolkow, maxKartKociolek);

            /* testy skutecznosci graczy
            gracz 1 - gra strategia podstawowa, tzn. wykonuje proste posuniecie, biorac pierwsza karte
            gracz 2 - gra strategia "wybor optymalnej karty" (unika eksplozji)
            */

            /*if (aktualnyGracz == 1)
                wybierzNajmniejszaKarte(gracze, iloscGraczy, &aktualnyGracz, koc, iloscKociolkow, maxKartKociolek);
            else if (aktualnyGracz == 2)
                wybierzOptymalnaKarte(gracze, iloscGraczy, &aktualnyGracz, koc, iloscKociolkow, maxKartKociolek);
            else if (aktualnyGracz == 3)
                prostePosuniecie(gracze, iloscGraczy, &aktualnyGracz, koc, iloscKociolkow, maxKartKociolek);*/

            wypiszRozdanie(plik, gracze, iloscGraczy, aktualnyGracz, maxKartKociolek);
            wypiszZawartosciKociolkow(plik, koc, iloscKociolkow);
            fclose(plik);

        }

        koniecRundy(gracze, iloscGraczy, koc, iloscKociolkow, iloscWygranych);
        runda++;
        rozdajacy++;
    }

    //wypiszStatystykiGraczy (iloscWygranych, iloscGraczy);

    delete[] wartosciKart;
    delete[] iloscWygranych;
    delete[] gracze;
    delete[] koc;
}
