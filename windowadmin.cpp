#include "windowadmin.h"
#include "ui_windowadmin.h"
#include <QMessageBox>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

extern int idQ;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
WindowAdmin::WindowAdmin(QWidget *parent):QMainWindow(parent),ui(new Ui::WindowAdmin)
{
    ui->setupUi(this);
    ::close(2);


    MESSAGE MsgConnect;


    // Recuperation de l'identifiant de la file de messages
    fprintf(stderr, "(ADMINISTRATEUR %d) Recuperation de l'id de la file de messages\n", getpid());

    if ((idQ = msgget(CLE, 0)) == -1)
    {
      fprintf(stderr, "\n(ADMINISTRATEUR %d) Erreur de msgget", getpid());
      exit(1);
    }
    MsgConnect.type = 1;
    MsgConnect.expediteur = getpid();
    MsgConnect.requete = LOGIN_ADMIN;

    if (msgsnd(idQ, &MsgConnect, sizeof(MESSAGE) - sizeof(long), 0) == -1)
    {
      perror("(ADMINISTRATEUR) Erreur de msgsnd");
      exit(1);
    }
    fprintf(stderr, "(ADMINISTRATEUR %d) attente d'une réponse\n", getpid());

    if (msgrcv(idQ, &MsgConnect, sizeof(MESSAGE) - sizeof(long), getpid(), 0) == -1)
    {
      perror("(ADMINISTRATEUR) Erreur de msgrcv");
      exit(1);
    }
    if(strcmp(MsgConnect.data1,"KO")==0){
      printf("(ADMINISTRATEUR %d) Erreur: un autre admin est déjà connecté\n", getpid());
      exit(1);
    }
    else if (strcmp(MsgConnect.data1, "OK") == 0){
      printf("(ADMINISTRATEUR %d) Connexion Réussi\n", getpid());
    }
}

WindowAdmin::~WindowAdmin()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions utiles : ne pas modifier /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowAdmin::setNom(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditNom->clear();
    return;
  }
  ui->lineEditNom->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* WindowAdmin::getNom()
{
  strcpy(nom,ui->lineEditNom->text().toStdString().c_str());
  return nom;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowAdmin::setMotDePasse(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditMotDePasse->clear();
    return;
  }
  ui->lineEditMotDePasse->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* WindowAdmin::getMotDePasse()
{
  strcpy(motDePasse,ui->lineEditMotDePasse->text().toStdString().c_str());
  return motDePasse;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowAdmin::setTexte(const char* Text)
{
  if (strlen(Text) == 0 )
  {
    ui->lineEditTexte->clear();
    return;
  }
  ui->lineEditTexte->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* WindowAdmin::getTexte()
{
  strcpy(texte,ui->lineEditTexte->text().toStdString().c_str());
  return texte;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowAdmin::setNbSecondes(int n)
{
  char Text[10];
  sprintf(Text,"%d",n);
  ui->lineEditNbSecondes->setText(Text);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WindowAdmin::getNbSecondes()
{
  char tmp[10];
  strcpy(tmp,ui->lineEditNbSecondes->text().toStdString().c_str());
  return atoi(tmp);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions permettant d'afficher des boites de dialogue /////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowAdmin::dialogueMessage(const char* titre,const char* message)
{
   QMessageBox::information(this,titre,message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowAdmin::dialogueErreur(const char* titre,const char* message)
{
   QMessageBox::critical(this,titre,message);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// Fonctions clics sur les boutons ////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WindowAdmin::on_pushButtonAjouterUtilisateur_clicked()
{
  // TO DO
  MESSAGE m;
  m.type=1;
  m.expediteur=getpid();
  m.requete=NEW_USER;
  if(!strcmp(getNom(), "-1")){
    dialogueErreur("Ajout Utilisateur", "Nom d'utilisateur -1 réservé!!!");
    return;
  }
  strcpy(m.data1,getNom());
  strcpy(m.data2,getMotDePasse());
  
  if (msgsnd(idQ, &m, sizeof(MESSAGE) - sizeof(long), 0) == -1)
  {
    perror("(Admin) Erreur de msgsnd");
    exit(1);
  }

  if (msgrcv(idQ, &m, sizeof(MESSAGE) - sizeof(long), getpid(), 0) == -1)
  {
    perror("(ADMINISTRATEUR) Erreur de msgrcv");
    exit(1);
  }

  setNom("");
  setMotDePasse("");
  
  if(strcmp(m.data1,"OK")==0)
  {
    dialogueMessage("Ajout Utilisateur",m.texte);
  }
  else if(strcmp(m.data1,"KO")==0)
  {
    dialogueErreur("Ajout Utilisateur",m.texte);
  }
}

void WindowAdmin::on_pushButtonSupprimerUtilisateur_clicked()
{
  // TO DO
  MESSAGE m;
  m.type=1;
  m.expediteur=getpid();
  m.requete=DELETE_USER;
  strcpy(m.data1,getNom());
  
  if (msgsnd(idQ, &m, sizeof(MESSAGE) - sizeof(long), 0) == -1)
  {
    perror("(Admin) Erreur de msgsnd");
    exit(1);
  }

  if (msgrcv(idQ, &m, sizeof(MESSAGE) - sizeof(long), getpid(), 0) == -1)
  {
    perror("(ADMINISTRATEUR) Erreur de msgrcv");
    exit(1);
  }

  setNom("");
  setMotDePasse("");
  
  if(strcmp(m.data1,"OK")==0)
  {
    dialogueMessage("Suppression Utilisateur",m.texte);
  }
  else if(strcmp(m.data1,"KO")==0)
  {
    dialogueErreur("Suppression Utilisateur",m.texte);
  }
}

void WindowAdmin::on_pushButtonAjouterPublicite_clicked()
{
  // TO DO
  MESSAGE m;
  m.type = 1;
  m.expediteur = getpid();
  m.requete = NEW_PUB;
  snprintf(m.data1, sizeof(m.data1), "%d", getNbSecondes());
  strcpy(m.texte, getTexte());
  if (msgsnd(idQ, &m, sizeof(MESSAGE) - sizeof(long), 0) == -1)
  {
    perror("(Admin) Erreur de msgsnd");
    exit(1);
  }
  setNbSecondes(0);
  setTexte("");
  
}

void WindowAdmin::on_pushButtonQuitter_clicked()
{
  MESSAGE Msglogout;
  Msglogout.type = 1;
  Msglogout.expediteur = getpid();
  Msglogout.requete = LOGOUT_ADMIN;
  if (msgsnd(idQ, &Msglogout, sizeof(MESSAGE) - sizeof(long), 0) == -1)
  {
    perror("(ADMINISTRATEUR) Erreur de msgsnd");
    exit(1);
  }
  exit(1);
}

