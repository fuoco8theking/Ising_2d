// ANALISI DATI ISING
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define nVariabili 2 // da cambiare se si vuole importare più variabili
#define lj 5
double **F;
int L;
int errore_k = 0;   // 0 = false, 1 = true
int b_errore_k = 0; // file da analizzare: esempio n20.txt --> b_errore_k = 20

int conta_iterazioni(
    const char *nomefile) { // restituisce iterazioni come variabile globale
  int temp1; // L (non mi serve perché lo inserisco da terminale (è necessario
             // inserirlo da terminale per acchiappare il file giusto da
             // leggere), beta (è relativo solo al file n0, lo leggo nella
             // funzione iterazione_su_file, che viene richiamata per ogni beta)
  double temp2;
  char nome_intero[100];
  int iterazioni;
  sprintf(nome_intero, "%s/L%d/n0.txt", nomefile, L);
  FILE *fp = fopen(nome_intero, "r");
  if (fp == NULL) {
    printf("ERRORE nell'apertura del file generazione_dati/L%d/n0.txt - "
           "funzione: proprieta_file --> Siete sicuri della scelta di L ? \n",
           L);
    return EXIT_FAILURE;
  }
  fscanf(fp, "%d %lf %d \n", &temp1, &temp2, &iterazioni);
  return iterazioni;
}

void jackknife(int k, int h, double **jack_array) {
  double sum_e = 0, sum2_e = 0;
  double sum_m = 0, sum2_m = 0, sum4_m = 0;
  double x_e = 0, x2_e = 0;
  double x_m = 0, x2_m = 0, x4_m = 0;
  int r;

  // somma delle quantità di mio interesse e ricavo media
  for (int i = 0; i < k * h; // k*h = iterazioni (forse)
       i++) {                // sommo x e x^2 di tutti i campionamenti
    sum_e += F[i][0];
    sum_m += F[i][1];

    sum2_e += F[i][0] * F[i][0];
    sum2_m += F[i][1] * F[i][1];

    sum4_m += F[i][1] * F[i][1] * F[i][1] * F[i][1];
  }

  // INIZIA JACKKNIFE SU K INSERITO DA UTENTE

  for (int i = 0; i < h; i++) {
    x_e = sum_e;
    x_m = sum_m;
    x2_e = sum2_e;
    x2_m = sum2_m;
    x4_m = sum4_m;

    for (int j = 0; j < k; j++) {
      r = j + i * k;
      x_e = x_e - F[r][0];
      x2_e = x2_e - pow(F[r][0], 2);
      x_m = x_m - F[r][1];
      x2_m = x2_m - pow(F[r][1], 2);
      x4_m = x4_m - pow(F[r][1], 4);
    }
    x_e = (double)x_e / (double)((h - 1) * k);
    x2_e = (double)x2_e / (double)((h - 1) * k);
    x_m = (double)x_m / (double)((h - 1) * k);
    x2_m = (double)x2_m / (double)((h - 1) * k);
    x4_m = (double)x4_m / (double)((h - 1) * k);

    jack_array[i][0] = x_e;                        // energia
    jack_array[i][1] = x_m;                        // |magnetizzazione|
    jack_array[i][2] = L * L * (x2_m - x_m * x_m); // suscettività
    jack_array[i][3] = x4_m / (x2_m * x2_m);       // cumulante
    jack_array[i][4] = L * L * (x2_e - x_e * x_e); // calore specifico
  } // fine ciclo su "i"

} // FINE JAKKNIFE

int input_dati(int contatore, int term, int sample, double *b,
               const char *nomefile) { // calcola tutte le funzioni
  FILE *file;
  char titolo[100];
  char filename[100];
  int temp1, temp2;

  sprintf(filename, "%s/L%d/n%d.txt", nomefile, L, contatore);
  if (errore_k == 1) {
    sprintf(filename, "%s/L%d/n%d.txt", nomefile, L, b_errore_k);
  }
  file = fopen(filename, "r");
  if (file == NULL) {
    printf("ising_analisi_dati: iterazione_su_file: WARNING - errore apertura: "
           "generazione_dati/L%d/n%d.txt --> IGNORA se generati %d file\n",
           L, contatore, contatore - 1);
    return 1; // Termina la funzione -> fa uscire dal while nel main
  }

  // salto titolo (leggendo beta) e termalizzazione
  fscanf(file, "%d %lf %d \n", &temp1, &(*b),
         &temp2); // salto la prima riga leggendo beta
  for (long int jump = 0; jump < term; jump++) {
    fgets(titolo, sizeof(titolo), file);
  }

  // input dati
  for (long int i = 0; i < sample; i++) {
    if (fscanf(file, "%lf %lf\n", &F[i][0], &F[i][1]) != 2) {
      fprintf(stderr, "Errore lettura dati alla riga %ld\n", i);
      break;
    }
  }

  fclose(file);
  // --- FINE IMPORT---

  if (errore_k == 1) {
    return 1; // nel caso di analisi per errore_k, analizzo un solo file
  }
  return 0; //  -> il ciclo while nel main continua
}

int main(int argc, char **argv) {
  int iterazioni, salto;
  int fine_file = 0, contatore_file = 0;
  double **jack_array;
  double media[lj], sigma[lj];
  double beta;
  int k, h;
  char datafile_import_root[80], datafile_export_root[80],
      datafile_export_intero[80];
  FILE *file;

  if (argc != 8) {
    printf("Il numero di variabili inserite è errato: inserire in ordine: "
           "L termalizzazione  lunghezza_k_binning   "
           "nome_file_import  nome_file_export  errore_k  b_errore_k\n");
    return EXIT_FAILURE;
  } else {
    L = atoi(argv[1]);
    salto = atoi(argv[2]);
    k = atoi(argv[3]);
    strcpy(datafile_import_root, argv[4]);
    strcpy(datafile_export_root, argv[5]);
    errore_k = atoi(argv[6]);
    b_errore_k = atoi(argv[7]);
    printf("Dati inseriti (term, k, datafile_import, datafile_export, "
           "errore_k, b_errore_k): %d, %d, "
           "%d, %s, %s, %d, %d\n",
           L, salto, k, datafile_import_root, datafile_export_root, errore_k,
           b_errore_k);
  }
  iterazioni = conta_iterazioni(datafile_import_root);
  h = (int)(iterazioni - salto) / k; //     // h è il numero di blocchi
  iterazioni =
      k * h; // in questo sto usando solo "la parte divisibile per k" dei dati

  // ALLOCAZIONE DINAMICA DELLA MEMORIA
  F = malloc((unsigned long int)iterazioni * sizeof(double *));
  if (F == NULL) {
    perror("Errore allocazione memoria F (prima dimensione)");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < iterazioni; i++) {
    F[i] = malloc((unsigned long int)nVariabili * sizeof(double));
    if (F[i] == NULL) {
      perror("Errore allocazione memoria F[i] (seconda dimensione)");
      exit(EXIT_FAILURE);
    }
  }

  jack_array = malloc((unsigned long int)h * sizeof(double *));
  if (jack_array == NULL) {
    perror("Errore allocazione memoria F (prima dimensione)");
    exit(EXIT_FAILURE);
  }
  for (long int i = 0; i < h; i++) {
    jack_array[i] = malloc((unsigned long int)lj * sizeof(double));
    if (jack_array[i] == NULL) {
      perror("Errore allocazione memoria F[i] (seconda dimensione)");
      exit(EXIT_FAILURE);
    }
  }
  // FINE ALLOCAZIONE DINAMICA

  sprintf(datafile_export_intero, "%s/L%d.txt", datafile_export_root, L);
  file = fopen(datafile_export_intero, "a"); // in questo modo dati "appesi"
  if (file == NULL) {
    fprintf(stderr, "Errore nell'aprire il file dati %s",
            datafile_export_intero);
  }
  fprintf(file, "# beta[0]   e[1]  sigma_e[2]   m[3]  sigma_m[4]  susc[5] "
                "sigma_susc[6]  cu[7] sigma_cu[8] cal[9] sigma_cal[10]\n");

  // inizio while
  while (fine_file == 0) {
    // INPUT DATI
    // input_dati(datafile_import, salto, iterazioni, &beta);
    fine_file = input_dati(contatore_file, salto, iterazioni, &beta,
                           datafile_import_root);
    contatore_file++; // istruzione da mettere dopo iterazione_su_file
    if (fine_file == 0 ||
        errore_k ==
            1) { // senza questo if l'ultimo file verrebbe analizzato due volte
      jackknife(k, h, jack_array);

      // calcolo media
      for (int j = 0; j < lj; j++) {
        media[j] = 0;
        for (long int i = 0; i < h; i++) {
          media[j] += jack_array[i][j];
        }
        media[j] = media[j] / (double)h;
      }

      // calcolo sigma
      for (int j = 0; j < lj; j++) {
        sigma[j] = 0;
        for (long int i = 0; i < h; i++) {
          sigma[j] += pow(media[j] - jack_array[i][j], 2);
        }
        sigma[j] = sqrt((sigma[j] * ((double)h - 1.0)) / (double)h);
      }

      // "# beta[0]   e[1]  sigma_e[2]   m[3]  sigma_m[4]  susc[5] sigma_susc[6]
      // cu[7] sigma_cu[8] cal[9] sigma_cal[10]
      fprintf(file,
              "%.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf "
              "%.12lf %.12lf %.12lf %.12lf \n",
              beta, media[0], sigma[0], media[1], sigma[1], media[2], sigma[2],
              media[3], sigma[3], media[4], sigma[4]);

    } // fine condizione if
  } // fine while
  fclose(file);

  // LIBERO MEMORIA ALLOCATA DINAMICAMENTE
  for (int i = 0; i < iterazioni; i++) {
    free(F[i]);
  }
  free(F);
  for (long int i = 0; i < h; i++)
    free(jack_array[i]);
  free(jack_array);

  printf("\nising_analisi_dati: FINE ESECUZIONE Analisi per L%d\n", L);
}
