// ISING CON CLUSTER UPDATE
// ultima modifica 23/93/26 ore 10:54 Andrea: aggiunta descrizioni
#include "pcg_basic.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* --- VALORI DA MODIFICARE ---
#define L 4  // larghezza reticolo
#define iterazioni 524288  //  2^10 = 1024 , 2^14 = 16384 , 2^15 = 32768  2^17 =
131072  2^19 = 524288 #define lunghezza_ciclo_scorrelazione 15 #define
beta_iniziale   0.000000 #define beta_finale     0.810000 #define
beta_incremento 0.04
*/
// --- FINE VALORI DA MODIFICARE ---

int L, iterazioni, lunghezza_ciclo_scorrelazione;
/*double beta_iniziale, beta_incremento, beta_finale;*/

int **web;
int **cluster_web;
int cluster_id =
    0; // invece di reinizializzare la matrice ausiliaria ogni volta, faccio in
       // modo che ad ogni ciclo il valore dei punti aggiunti al cluster aumenti
       // di uno, in questo modo riassegno solo i punti aggiornati
int S;
double Padd;
pcg32_random_t rng;

int rand_int(int N) { // genera un numero casuale intero tra 0 ed N-1
  // return random() % N;
  return pcg32_boundedrand_r(&rng, N);
}

double rand_double(
    double min,
    double max) { // genera un numero casuale float compreso tra [min,max)
  // return ((double)random() / RAND_MAX) * (max - min) + min;
  return ldexp(pcg32_random_r(&rng), -32) * (max - min) + min;
}

double energia() {
  double en = 0.0;
  for (int i = 0; i < L; i++) {
    for (int j = 0; j < L; j++) {
      en += web[i][j] * (web[(i + 1) % L][j] + web[i][(j + 1) % L]);
    }
  }
  return -en;
}

double magnetizzazione_abs() {
  double spintot = 0;
  for (int i = 0; i < L; i++) {
    for (int j = 0; j < L; j++) {
      spintot += web[i][j];
    }
  }
  return fabs(spintot);
}

int agg(int x) { // sposta l'indice a destra/in alto rispettando le cc (su L-1)
  if (x == L - 1) {
    x = 0;
  } else {
    x = x + 1;
  }
  return x;
}

int rim(int x) { // sposta indice a sinistra/in basso rispettando le cc (su 0)
  if (x == 0) {
    x = L - 1;
  } else {
    x = x - 1;
  }
  return x;
}

void cluster(int x, int y) {      // in ingresso var che definiscono il sito
  cluster_web[x][y] = cluster_id; // questo potrebbe stare dentro le funzioni,
                                  // ma almeno lo scrivo una sola volta

  // valuto sopra
  if (cluster_web[x][agg(y)] != cluster_id && web[x][agg(y)] == S) {
    if (rand_double(0, 1) < Padd) {
      cluster(x, agg(y));
    }
  }
  // valuto destra
  if (cluster_web[agg(x)][y] != cluster_id && web[agg(x)][y] == S) {
    if (rand_double(0, 1) < Padd) {
      cluster(agg(x), y);
    }
  }
  // valuto giù
  if (cluster_web[x][rim(y)] != cluster_id && web[x][rim(y)] == S) {
    if (rand_double(0, 1) < Padd) {
      cluster(x, rim(y));
    }
  }
  // valuto sinistra
  if (cluster_web[rim(x)][y] != cluster_id && web[rim(x)][y] == S) {
    if (rand_double(0, 1) < Padd) {
      cluster(rim(x), y);
    }
  }

} // fine cluster

int main(int argc, char **argv) {
  FILE *file;
  char filename[100];
  char root_folder_gen[80];
  int u, v;
  int contatore_cluster = 0,
      contatore_nomefile = 0; // contatore_nomefile lo faccio partire da -1 in
                              // modo che parta da 0 il nome effettivo
  int lung_ciclo_scorrelazione;
  double b;
  double b_fine;
  double b_incremento;

  pcg32_srandom_r(&rng, time(NULL),
                  (intptr_t)&rng); // genera seme per numeri casuali

  if (argc != 8) {
    printf("ising_cluster: ERRORE - il numero di dati inseriti da riga di "
           "comando è errato!\n");
    printf("Inserire: L iterazioni lunghezza_scorr  b   b_fine  db  "
           "root_folder\n");
    printf("esempio: ./generazione 40 100000 7 0.420 0.435 0.002 "
           "~/generazione_mod1\n");
    printf("in questo caso un esempio di file generato è "
           "~/generazione_mod1/L40/n#.txt\n");
    printf("Assicurarsi che la cartella inserita esista e anche le relative "
           "sottocartelle per gli L#\n");
    return EXIT_FAILURE;
  } else {
    // NUMERI INSERITI DA RIGA DI COMANDO
    L = atoi(argv[1]);
    iterazioni = atoi(argv[2]);
    lunghezza_ciclo_scorrelazione = atoi(argv[3]);
    b = atof(argv[4]);
    b_fine = atof(argv[5]);
    b_incremento = atof(argv[6]);
    strcpy(root_folder_gen,
           argv[7]); // copio il nome del file fornito dall'esterno
  }
  printf("Valori inseriti: %d %d %d %lf %lf %lf\n", L, iterazioni,
         lunghezza_ciclo_scorrelazione, b, b_fine, b_incremento);
  lung_ciclo_scorrelazione = lunghezza_ciclo_scorrelazione;

  clock_t inizio = clock();

  // ALLOCAZIONE DINAMICA DELLA MEMORIA

  web = malloc((unsigned long int)L * sizeof(double *));
  if (web == NULL) {
    perror("Errore allocazione memoria F (prima dimensione)");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < L; i++) {
    web[i] = malloc((unsigned long int)L * sizeof(double));
    if (web[i] == NULL) {
      perror("Errore allocazione memoria F[i] (seconda dimensione)");
      exit(EXIT_FAILURE);
    }
  }

  cluster_web = malloc((unsigned long int)L * sizeof(double *));
  if (cluster_web == NULL) {
    perror("Errore allocazione memoria F (prima dimensione)");
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < L; i++) {
    cluster_web[i] = malloc((unsigned long int)L * sizeof(double));
    if (cluster_web[i] == NULL) {
      perror("Errore allocazione memoria F[i] (seconda dimensione)");
      exit(EXIT_FAILURE);
    }
  }

  // FINE ALLOCAZIONE DINAMICA

  // printf("MODELLO DI ISING CON UPDATE A CLUSTER - L%d\n", L);
  // printf("ATTENZIONE: chiudere tutti i file dati prima di eseguire\n");

  // inizializzo lattice cluster a 0
  for (int i = 0; i < L; i++) {
    for (int j = 0; j < L; j++) {
      cluster_web[i][j] = 0;
    }
  }

  do { // inizio iterazione al variare di BETA

    // INIZIALIZZO RETE CON SPIN RANDOM
    for (int i = 0; i < L; i++) {
      for (int j = 0; j < L; j++) {
        web[i][j] = rand_int(2) * 2 - 1; // genero casualmente 1 e -1
      }
    }

    contatore_cluster = 0;
    Padd = 1 - exp(-2 * b); // inizializzo Padd(b)
    sprintf(
        filename, "%s/L%d/n%d.txt", root_folder_gen, L,
        contatore_nomefile); // inizializzo nome del file su cui scrivere dati
    file = fopen(filename, "w");
    if (file != NULL) {
      fprintf(file, "%d %lf %d \n", L, b,
              iterazioni); // nella prima riga salvo, in ordine: lato reticolo -
                           // beta - lunghezza vettore dati
      fclose(file);
    } // cancello eventuali dati già presenti
    else {
      printf("errore nell'apertura del file per inizializzazione!");
    }
    file = fopen(filename, "a"); // riapro il file in modalità append

    // FINE INIZIALIZZAZIONE

    // CLUSTER UPDATE
    for (int q = 0; q < iterazioni; q++) { // inizio ciclo iterazioni
      for (int i = 0; i < lung_ciclo_scorrelazione;
           i++) { // inizio ciclo 10 update cluster
        u = rand_int(L);
        v = rand_int(L); // scelta casuale del sito

        S = web[u][v];
        cluster(u, v);
        // cluster_iterativo(u,v);

        // aggiorno i valori della matrice usando il cluster creato
        for (int i = 0; i < L; i++) {
          for (int j = 0; j < L; j++) {
            if (cluster_web[i][j] == cluster_id) {
              web[i][j] =
                  -web[i][j]; // flippo il valore dello spin nel cluster creato
              contatore_cluster++;
            }
          }
        }
        cluster_id++;
      } // fine ciclo "lung_ciclo_scorrelazione" update cluster

      // salvataggio_dati(b, filename);
      fprintf(file, "%.12lf %.12lf \n", energia() / (L * L),
              magnetizzazione_abs() / (L * L)); // salvo i dati
    } // fine ciclo iterazioni

    // printf("Dimensione media cluster (beta = %lf): %lf / %d \n", b,
    // (double)contatore_cluster/ (double)(lung_ciclo_scorrelazione*iterazioni),
    // L*L);

    b += b_incremento; // incremento di beta, da modificare all'occorrenza
    contatore_nomefile++;
    fclose(file); // chiudo il file appena creato

  } while (b <= b_fine);

  // LIBERO MEMORIA ALLOCATA DINAMICAMENTE web e cluster_web
  for (int i = 0; i < L; i++) {
    free(web[i]);
    free(cluster_web[i]);
  }
  free(web);
  free(cluster_web);
  // fine libera memoria

  clock_t fine = clock();
  double tempo = (double)(fine - inizio) / CLOCKS_PER_SEC;
  printf("ising_cluster: Tempo di esecuzione (L%d): %.3f secondi\n", L, tempo);
  // printf("Il codice è stato eseguito con successo!\n");
}
