#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define NAME_LEN 30
#define NUM_PLAYERS 12
#define TEAM_SIZE 3
#define NUM_ABILITIES 3
#define DELAY 1800000
#define SMALL_DELAY 15000

int currRound;
int dI = 0;
int cpuDI = 0;


typedef struct abilities {
    char abilityName[NAME_LEN];
    int damage;
    int cursedEnergyConsumption;
} abilities;

typedef struct domainExpansion {
    char domainName[NAME_LEN];
    int damage;
    int cursedEnergyConsumption;
    char *fileName;
} domainExpansion;

typedef struct player {
    char name[NAME_LEN];
    abilities *arsenal[NUM_ABILITIES];
    domainExpansion *domain;
    int abilityUsed[NUM_ABILITIES + 1];
} player;

typedef struct team {
    player *roster[TEAM_SIZE];
    int cursedEnergy;
    int health;
} team;

typedef struct tournamentBracket {
    team *team1;
    team *team2;
    team *winner;
} tournamentBracket;

void welcomeScreen();
void rules();
player *createPlayers();
team *chooseTeams(player *playerList);
char *createFileName(char *playerName);
void readFromFile(char *fileName);
void printTeamAbilities(team *userTeam);
team *createCPUTeam(player *playerList);
void playGame(team *userTeam, team *cpuTeam);
void freePlayerList(player *playerList);
void freeTeams(team *aTeam);
void printPlayerAbilities(player *player);
void cpuPlayGame(team *userTeam, team *cpuTeam, abilities *userAbility, int userIdx, domainExpansion *userDomain, int userBlock, 
    int userHeal, int round);
void battleGround(team *userTeam, team *cpuTeam, abilities *userAbility, int userIdx, domainExpansion *userDomain, int userBlock, 
    int userHeal, abilities *cpuAbility, int cpuIdx, domainExpansion *cpuDomain, int cpuBlock, int cpuHeal);
void bG1(team *userTeam, team *cpuTeam, abilities *userAbility, abilities *cpuAbility);
void bG2(team *userTeam, team *cpuTeam, abilities *userAbility, int cpuBlock);
void bG3(team *userTeam, team *cpuTeam, abilities *userAbility, int cpuHeal);
void bG4(team *userTeam, team *cpuTeam, abilities *userAbility, domainExpansion *cpuDomain);
void bG5(team *userTeam, team *cpuTeam, int userBlock, abilities *cpuAbility);
void bG6(team *userTeam, team *cpuTeam, int userBlock, int cpuBlock);
void bG7(team *userTeam, team *cpuTeam, int userBlock, int cpuHeal);
void bG8(team *userTeam, team *cpuTeam, int userBlock, domainExpansion *cpuDomain);
void bG9(team *userTeam, team *cpuTeam, int userHeal, abilities *cpuAbility);
void bG10(team *userTeam, team *cpuTeam, int userHeal, int cpuBlock);
void bG11(team *userTeam, team *cpuTeam, int userHeal, int cpuHeal);
void bG12(team *userTeam, team *cpuTeam, int userHeal, domainExpansion *cpuDomain);
void bG13(team *userTeam, team *cpuTeam, domainExpansion *userDomain, abilities *cpuAbility);
void bG14(team *userTeam, team *cpuTeam, domainExpansion *userDomain, int cpuBlock);
void bG15(team *userTeam, team *cpuTeam, domainExpansion *userDomain, int cpuHeal);
void bG16(team *userTeam, team *cpuTeam, domainExpansion *userDomain, domainExpansion *cpuDomain);
void resetAbilities(player *p);
void useAbility(player *p, abilities *ability);
bool checkAbility(player *p, abilities *ability);
bool allUsed(player *p);
void useAll(player *p);
void domainIndicator(int *d);
void domainWasUsed(player *p);
bool canAttack(team *t);
bool canUseAnyAbility(team *t);
int useThisAbility(team *t);
void executeDomain(team *t, domainExpansion *dE);



int main() {
    welcomeScreen();
    rules();
    player *playerList = createPlayers();
    team *userTeam = chooseTeams(playerList);
    printTeamAbilities(userTeam);
    team *cpuTeam;
    cpuTeam = createCPUTeam(playerList);
    printf("\nCPU Team:\n");
    printTeamAbilities(cpuTeam);
    playGame(userTeam, cpuTeam);

    freeTeams(cpuTeam);
    freeTeams(userTeam);
    freePlayerList(playerList);
    return 0;
}

void welcomeScreen() {
    char a[] = "  JJJ U    U   JJJ  U    U TTTTT  SSSS  U    U       K  K     A     IIIII  SSSS  EEEEE NN    N";
    char b[] = "    J U    U     J  U    U   T   S      U    U       K K     A A      I   S      E     N N   N";
    char c[] = "    J U    U     J  U    U   T    SSSS  U    U       KK     AAAAA     I    SSSS  EEE   N  N  N";
    char d[] = "J   J U    U J   J  U    U   T        S U    U       K K   A     A    I        S E     N   N N";
    char e[] = " JJJ   UUUU   JJJ    UUUU    T    SSSSS  UUUU        K  K A       A IIIII  SSSS  EEEEE N    NN";

    char *lines[] = {a, b, c, d, e}; // Array of lines

    printf("Welcome to:\n");
    // Loop through each line of ASCII art
    for (int line = 0; line < 5; line++) {
        // Loop through each character of the line
        for (int i = 0; lines[line][i] != '\0'; i++) {
            printf("%c", lines[line][i]);
            fflush(stdout); // Flush output buffer to display immediately
            usleep(1000); // Delay in microseconds (0.001 second)
        }
        printf("\n"); // Move to the next line
    }

}

void rules() {
    printf("\nRULES:\n");
    printf("- You will be choosing a team of three players\n");
    printf("- You will use one player at a time\n");
    printf("- Each team will have the same amount of health and cursed energy\n");
    printf("- Each turn, you have the option to attack, block, or heal, each of which will cost cursed energy\n");
    printf("- Each player can only use each offensive ability once per cycle. This includes domain expansions\n");
    printf("- Once all abilities have been used, the cycle resets and all abilities are useable again\n");
    printf("- When blocking or healing, you can choose how much of your cursed energy you want to use\n");
    printf("- If your amount exceeds what you have, you will use up all your remaining energy\n");
    printf("- For characters with a domain expansion, you cannot use any abilities for one turn after using domain expansion\n");
    printf("- After the turn ends, your abilities will go back to what their availability was before using the domain expansion\n");
}

player *createPlayers() {
    player *playerList = (player*)malloc(NUM_PLAYERS * sizeof(player));
    
    for (int i = 0; i < NUM_PLAYERS; i++) {
        playerList[i].domain = (domainExpansion*)malloc(sizeof(domainExpansion));
        for (int j = 0; j < NUM_ABILITIES; j++) {
            playerList[i].arsenal[j] = (abilities*)malloc(sizeof(abilities));
        }
    }

    strcpy(playerList[0].name, "Todo");
    strcpy(playerList[0].arsenal[0]->abilityName, "Boogie Woogie");
    playerList[0].arsenal[0]->cursedEnergyConsumption = 15000;
    playerList[0].arsenal[0]->damage = 5000;
    strcpy(playerList[0].arsenal[1]->abilityName, "Black Flash");
    playerList[0].arsenal[1]->cursedEnergyConsumption = 30000;
    playerList[0].arsenal[1]->damage = 10000;
    strcpy(playerList[0].arsenal[2]->abilityName, "Punch Barage");
    playerList[0].arsenal[2]->cursedEnergyConsumption = 12500;
    playerList[0].arsenal[2]->damage = 4166;
    playerList[0].domain = NULL;

    strcpy(playerList[1].name, "Hanami");
    strcpy(playerList[1].arsenal[0]->abilityName, "Cursed Buds");
    playerList[1].arsenal[0]->cursedEnergyConsumption = 20000;
    playerList[1].arsenal[0]->damage = 6666;
    strcpy(playerList[1].arsenal[1]->abilityName, "Roots");
    playerList[1].arsenal[1]->cursedEnergyConsumption = 13000;
    playerList[1].arsenal[1]->damage = 4333;
    strcpy(playerList[1].arsenal[2]->abilityName, "Wooden Balls");
    playerList[1].arsenal[2]->cursedEnergyConsumption = 10000;
    playerList[1].arsenal[2]->damage = 3333;
    playerList[1].domain = NULL;

    strcpy(playerList[2].name, "Jogo");
    strcpy(playerList[2].arsenal[0]->abilityName, "Disaster Flames");
    playerList[2].arsenal[0]->cursedEnergyConsumption = 25000;
    playerList[2].arsenal[0]->damage = 8333;
    strcpy(playerList[2].arsenal[1]->abilityName, "Ember Insects");
    playerList[2].arsenal[1]->cursedEnergyConsumption = 20000;
    playerList[2].arsenal[1]->damage = 6666;
    strcpy(playerList[2].arsenal[2]->abilityName, "Maximum: Meteor");
    playerList[2].arsenal[2]->cursedEnergyConsumption = 45000;
    playerList[2].arsenal[2]->damage = 15000;
    strcpy(playerList[2].domain->domainName, "Coffin of the Iron Mountain");
    playerList[2].domain->cursedEnergyConsumption = 79000;
    playerList[2].domain->damage = 26333;
    playerList[2].domain->fileName = createFileName(playerList[2].name);

    strcpy(playerList[3].name, "Nanami");
    strcpy(playerList[3].arsenal[0]->abilityName, "Black Flash");
    playerList[3].arsenal[0]->cursedEnergyConsumption = 30000;
    playerList[3].arsenal[0]->damage = 10000;
    strcpy(playerList[3].arsenal[1]->abilityName, "Ratio Technique");
    playerList[3].arsenal[1]->cursedEnergyConsumption = 12000;
    playerList[3].arsenal[1]->damage = 4000;
    strcpy(playerList[3].arsenal[2]->abilityName, "Overtime Barage");
    playerList[3].arsenal[2]->cursedEnergyConsumption = 18750;
    playerList[3].arsenal[2]->damage = 6250;
    playerList[3].domain = NULL;

    strcpy(playerList[4].name, "Mahito");
    strcpy(playerList[4].arsenal[0]->abilityName, "Idle Transfiguration");
    playerList[4].arsenal[0]->cursedEnergyConsumption = 21000;
    playerList[4].arsenal[0]->damage = 7000;
    strcpy(playerList[4].arsenal[1]->abilityName, "Soul Multiplicity");
    playerList[4].arsenal[1]->cursedEnergyConsumption = 10000;
    playerList[4].arsenal[1]->damage = 3333;
    strcpy(playerList[4].arsenal[2]->abilityName, "Body Repel");
    playerList[4].arsenal[2]->cursedEnergyConsumption = 14000;
    playerList[4].arsenal[2]->damage = 4666;
    strcpy(playerList[4].domain->domainName, "Self-Embodiment of Perfection");
    playerList[4].domain->cursedEnergyConsumption = 80000;
    playerList[4].domain->damage = 26666;
    playerList[4].domain->fileName = createFileName(playerList[4].name);

    strcpy(playerList[5].name, "Megumi");
    strcpy(playerList[5].arsenal[0]->abilityName, "Divine Dog: Totality");
    playerList[5].arsenal[0]->cursedEnergyConsumption = 14000;
    playerList[5].arsenal[0]->damage = 4666;
    strcpy(playerList[5].arsenal[1]->abilityName, "Nue");
    playerList[5].arsenal[1]->cursedEnergyConsumption = 13500;
    playerList[5].arsenal[1]->damage = 4500;
    strcpy(playerList[5].arsenal[2]->abilityName, "Mahoraga");
    playerList[5].arsenal[2]->cursedEnergyConsumption = 70000;
    playerList[5].arsenal[2]->damage = 23333;
    strcpy(playerList[5].domain->domainName, "Chimera Shadow Garden");
    playerList[5].domain->cursedEnergyConsumption = 60000;
    playerList[5].domain->damage = 20000;
    playerList[5].domain->fileName = createFileName(playerList[5].name);
    
    strcpy(playerList[6].name, "Gojo");
    strcpy(playerList[6].arsenal[0]->abilityName, "Blue");
    playerList[6].arsenal[0]->cursedEnergyConsumption = 19000;
    playerList[6].arsenal[0]->damage = 6333;
    strcpy(playerList[6].arsenal[1]->abilityName, "Red");
    playerList[6].arsenal[1]->cursedEnergyConsumption = 25000;
    playerList[6].arsenal[1]->damage = 8333;
    strcpy(playerList[6].arsenal[2]->abilityName, "Hollow Purple");
    playerList[6].arsenal[2]->cursedEnergyConsumption = 50000;
    playerList[6].arsenal[2]->damage = 16666;
    strcpy(playerList[6].domain->domainName, "Infinite Void");
    playerList[6].domain->cursedEnergyConsumption = 88000;
    playerList[6].domain->damage = 29333;
    playerList[6].domain->fileName = createFileName(playerList[6].name);

    strcpy(playerList[7].name, "Geto");
    strcpy(playerList[7].arsenal[0]->abilityName, "Cursed Spirit Manipulation");
    playerList[7].arsenal[0]->cursedEnergyConsumption = 20000;
    playerList[7].arsenal[0]->damage = 6666;
    strcpy(playerList[7].arsenal[1]->abilityName, "Playful Cloud");
    playerList[7].arsenal[1]->cursedEnergyConsumption = 15000;
    playerList[7].arsenal[1]->damage = 5000;
    strcpy(playerList[7].arsenal[2]->abilityName, "Maximum: Uzumaki");
    playerList[7].arsenal[2]->cursedEnergyConsumption = 47000;
    playerList[7].arsenal[2]->damage = 15666;
    playerList[7].domain = NULL;
    
    strcpy(playerList[8].name, "Sukuna");
    strcpy(playerList[8].arsenal[0]->abilityName, "Cleave");
    playerList[8].arsenal[0]->cursedEnergyConsumption = 22000;
    playerList[8].arsenal[0]->damage = 7333;
    strcpy(playerList[8].arsenal[1]->abilityName, "Dismantle");
    playerList[8].arsenal[1]->cursedEnergyConsumption = 19000;
    playerList[8].arsenal[1]->damage = 6333;
    strcpy(playerList[8].arsenal[2]->abilityName, "Divine Flame Open");
    playerList[8].arsenal[2]->cursedEnergyConsumption = 55000;
    playerList[8].arsenal[2]->damage = 18333;
    strcpy(playerList[8].domain->domainName, "Malevolent Shrine");
    playerList[8].domain->cursedEnergyConsumption = 90000;
    playerList[8].domain->damage = 30000;
    playerList[8].domain->fileName = createFileName(playerList[8].name);

    strcpy(playerList[9].name, "Toji");
    strcpy(playerList[9].arsenal[0]->abilityName, "Split Soul Katana");
    playerList[9].arsenal[0]->cursedEnergyConsumption = 0;
    playerList[9].arsenal[0]->damage = 8000;
    strcpy(playerList[9].arsenal[1]->abilityName, "Inverted Spear of Heaven");
    playerList[9].arsenal[1]->cursedEnergyConsumption = 0;
    playerList[9].arsenal[1]->damage = 10000;
    strcpy(playerList[9].arsenal[2]->abilityName, "Playful Cloud");
    playerList[9].arsenal[2]->cursedEnergyConsumption = 0;
    playerList[9].arsenal[2]->damage = 6000;    
    playerList[9].domain = NULL;
    
    strcpy(playerList[10].name, "Yuji");
    strcpy(playerList[10].arsenal[0]->abilityName, "Divergent Fist");
    playerList[10].arsenal[0]->cursedEnergyConsumption = 10000;
    playerList[10].arsenal[0]->damage = 3333;
    strcpy(playerList[10].arsenal[1]->abilityName, "Black Flash");
    playerList[10].arsenal[1]->cursedEnergyConsumption = 30000;
    playerList[10].arsenal[1]->damage = 10000;
    strcpy(playerList[10].arsenal[2]->abilityName, "Consecutive Black Flashes");
    playerList[10].arsenal[2]->cursedEnergyConsumption = 38000;
    playerList[10].arsenal[2]->damage = 12666;
    playerList[10].domain = NULL;
    
    strcpy(playerList[11].name, "Choso");
    strcpy(playerList[11].arsenal[0]->abilityName, "Piercing Blood");
    playerList[11].arsenal[0]->cursedEnergyConsumption = 12000;
    playerList[11].arsenal[0]->damage = 4000;
    strcpy(playerList[11].arsenal[1]->abilityName, "Supernova");
    playerList[11].arsenal[1]->cursedEnergyConsumption = 14000;
    playerList[11].arsenal[1]->damage = 4666;
    strcpy(playerList[11].arsenal[2]->abilityName, "Slicing Exorcism");
    playerList[11].arsenal[2]->cursedEnergyConsumption = 13000;
    playerList[11].arsenal[2]->damage = 4333;
    playerList[11].domain = NULL;

    return playerList;
}

team *chooseTeams(player *playerList) {
    team *userTeam = (team*)malloc(sizeof(team));
    for (int i = 0; i < TEAM_SIZE; i++) {
        userTeam->roster[i] = (player*)malloc(sizeof(player));
        strcpy(userTeam->roster[i]->name, "Wsg");
    }
    userTeam->cursedEnergy = 300000;
    userTeam->health = 100000;
    printf("\nChoose three players to be on your team in order of your choice. First player will always start each game\n");
    for (int i = 0; i < NUM_PLAYERS; i++) {
        printf("%d. %s\n", i + 1, playerList[i].name);
    }

    char chosenPlayer[NAME_LEN];
    for (int i = 0; i < TEAM_SIZE; i++) {
        printf("Enter desired character name: \n");
        scanf("%s", chosenPlayer);
        if ((strcmp(chosenPlayer, userTeam->roster[0]->name) != 0) && (strcmp(chosenPlayer, userTeam->roster[1]->name) != 0) && (strcmp(chosenPlayer, userTeam->roster[2]->name) != 0)) {
            for (int j = 0; j < NUM_PLAYERS; j++) {
                if (strcmp(chosenPlayer, playerList[j].name) == 0) {
                    memcpy(userTeam->roster[i], &playerList[j], sizeof(player));
                    break;
                }
            }
        } else {
            printf("Player already chosen\n");
            printf("Try again\n\n");
            i--;
            continue;
        }
        for (int k = 0; k < NUM_PLAYERS + 1; k++) {
            if (strcmp(chosenPlayer, playerList[k].name) == 0) {
                break;
            }
            if (k == NUM_PLAYERS) {
                printf("Player does not exist\n");
                printf("Try again\n\n");
                i--;
                continue;
            }
        }
    }

    printf("Your team is:\n");
    for (int i = 0; i < TEAM_SIZE; i++) {
        printf("%d. %s\n", i + 1, userTeam->roster[i]->name);
    }

    return userTeam;
}

char *createFileName(char *playerName) {
    char *fileName = malloc(strlen(playerName) + 5);
    strcpy(fileName, playerName);
    strcat(fileName, ".txt");

    return fileName;
}

void readFromFile(char *fileName) {
    FILE *file = fopen(fileName, "r");

    char ascii_art[120];
    for (int i = 0; i < 120; i++) {
        fgets(ascii_art, sizeof(ascii_art), file);
        printf("%s", ascii_art);
    }

    fclose(file);
}

void printTeamAbilities(team *userTeam) {
    printf("\nTeam Cursed Energy: %d\n", userTeam->cursedEnergy);
    printf("Team Health: %d\n", userTeam->health);

    for (int i = 0; i < TEAM_SIZE; i++) {
        printf("Press 'Enter' to view player %d's abilities\n", i + 1);
        while (getchar() != '\n');
        printf("\n%s's abilities:\n", userTeam->roster[i]->name);
        for (int j = 0; j < NUM_ABILITIES; j++) {
            printf("\nAbility %d:\n", j + 1);
            printf("Name: %s\n", userTeam->roster[i]->arsenal[j]->abilityName);
            printf("Cursed Energy Consumption: %d\n", userTeam->roster[i]->arsenal[j]->cursedEnergyConsumption);
            printf("Damage: %d\n", userTeam->roster[i]->arsenal[j]->damage);
        }
        if (userTeam->roster[i]->domain != NULL) {
            printf("\nDomain Expansion:\n");
            printf("Name: %s\n", userTeam->roster[i]->domain->domainName);
            printf("Cursed Energy Consumption: %d\n", userTeam->roster[i]->domain->cursedEnergyConsumption);
            printf("Damage: %d\n", userTeam->roster[i]->domain->damage);
        }
    }
}

team *createCPUTeam(player *playerList) {
    team *CPUTeam = (team*)malloc(sizeof(team));
    for (int i = 0; i < TEAM_SIZE; i++) {
        CPUTeam->roster[i] = (player*)malloc(sizeof(player));
    }
    CPUTeam->cursedEnergy = 300000;
    CPUTeam->health = 100000;
    int r;
    srand(time(NULL));
    for (int i = 0; i < TEAM_SIZE; i++) {
        r = rand() % NUM_PLAYERS;
        if (i == 2){
            if ((strcmp(playerList[r].name, CPUTeam->roster[i - 2]->name) != 0) && 
            (strcmp(playerList[r].name, CPUTeam->roster[i - 1]->name) != 0)) {
                memcpy(CPUTeam->roster[i], &playerList[r], sizeof(player));
            } else {
                i--;
                continue;
            }
        } else if (i == 1) {
            if (strcmp(playerList[r].name, CPUTeam->roster[i - 1]->name) != 0) {
                memcpy(CPUTeam->roster[i], &playerList[r], sizeof(player));
            } else {
                i--;
                continue;
            }
        } else {
            memcpy(CPUTeam->roster[i], &playerList[r], sizeof(player));
        }
    }

    return CPUTeam;
}

void freePlayerList(player *playerList) {
    for (int i = 0; i < NUM_PLAYERS; i++) {
        free(playerList[i].domain->fileName);
        free(playerList[i].domain);
        for (int j = 0; j < NUM_ABILITIES; j++) {
            free(playerList[i].arsenal[j]);
        }
    }

    free(playerList);
}

void freeTeams(team *aTeam) {
    for (int i = 0; i < TEAM_SIZE; i++) {
        free(aTeam->roster[i]);
    }

    free(aTeam);
}

void playGame(team *userTeam, team *cpuTeam) {
    int winCount = 0;
    int lossCount = 0;
    for (int i = 0; i < TEAM_SIZE; i++) {
        resetAbilities(userTeam->roster[i]);
        resetAbilities(cpuTeam->roster[i]);
        currRound = i;
        userTeam->cursedEnergy = 350000;
        cpuTeam->cursedEnergy = 350000;
        userTeam->health = 100000;
        cpuTeam->health = 100000;
        printf("\nRound %d!\n", i + 1);
        printf("%s VERSUS %s\n", userTeam->roster[i]->name, cpuTeam->roster[i]);
        int c;
        int c2;
        abilities *chosenAbility;
        domainExpansion *chosenDomain;
        int chosenDefend;
        int chosenHeal;
        while ((userTeam->health > 0 && cpuTeam->health > 0) && (canAttack(userTeam) || canAttack(cpuTeam))) {
            domainIndicator(&dI);
            printf("\nYour Player's Health: %d\tYour Player's Cursed Energy: %d", userTeam->health, userTeam->cursedEnergy);
            printf("\nOpponent's Health: %d  \tOpponent's Cursed Energy: %d\n", cpuTeam->health, cpuTeam->cursedEnergy);
            printf("\nEnter which number is your plan of action!\n");
            printf("1. Attack\n2. Defend\n3. Heal\n");
            scanf("%d", &c);
            switch (c) {
                case 1:
                    if (dI == 0) {
                        if (allUsed(userTeam->roster[currRound])) {
                            resetAbilities(userTeam->roster[currRound]);
                        }
                        printf("\nEnter the number of the ability of your choice:\n");
                        printPlayerAbilities(userTeam->roster[i]);
                        scanf("%d", &c2);
                        while (userTeam->roster[currRound]->abilityUsed[c2 - 1] != 0 || (c2 < 1 || c2 > 4)) {
                            if (c2 < 1 || c2 > 4) {
                                printf("Invalid choice. Try again\n");
                            } else if (c2 == 4 && userTeam->roster[currRound]->domain == NULL) {
                                printf("You do not have a domain expansion. Try again\n");
                            } else {
                                printf("You have already used this ability in this cycle.\n Choose another option.\n");
                            }
                            printf("\nEnter the number of the ability of your choice:\n");
                            scanf("%d", &c2);
                        }
                        switch (c2) {
                            case 1:
                                chosenAbility = userTeam->roster[i]->arsenal[0];
                                cpuPlayGame(userTeam, cpuTeam, chosenAbility, 0, NULL, -1, -1, i);
                                break;
                            case 2:
                                chosenAbility = userTeam->roster[i]->arsenal[1];
                                cpuPlayGame(userTeam, cpuTeam, chosenAbility, 1, NULL, -1, -1, i);
                                break;
                            case 3:
                                chosenAbility = userTeam->roster[i]->arsenal[2];
                                cpuPlayGame(userTeam, cpuTeam, chosenAbility, 2, NULL, -1, -1, i);
                                break;
                            case 4:
                                chosenDomain = userTeam->roster[i]->domain;   
                                cpuPlayGame(userTeam, cpuTeam, NULL, -1, chosenDomain, -1, -1, i);
                                break;
                            default:
                                printf("Invalid choice. An option will be chosen for you.\n");
                                int r;
                                srand(time(NULL));
                                r = rand() % NUM_ABILITIES;
                                chosenAbility = userTeam->roster[i]->arsenal[r]; 
                                cpuPlayGame(userTeam, cpuTeam, chosenAbility, -1, NULL, -1, -1, i);                   
                        }
                        break;
                    } else {
                        printf("You used your Domain Expansion last round, therefore you cannot attack this round.\n");
                        printf("You will be prompted to block\n");
                    }
                case 2:
                    printf("Choose how much Cursed Energy you want to use to block. The amount of cursed energy you choose will be equivalent to the amount of damage you block\n");
                    scanf("%d", &chosenDefend);
                    cpuPlayGame(userTeam, cpuTeam, NULL, -1, NULL, chosenDefend, -1, i);
                    break;
                case 3:
                    printf("Choose how much Cursed Energy you want to use to heal. The amount of cursed energy you choose will be equivalent to the amount of health you heal.\n");
                    printf("You cannot use more than half of your cursed energy to heal (%d)\n", userTeam->cursedEnergy / 2);
                    scanf("%d", &chosenHeal);
                    cpuPlayGame(userTeam, cpuTeam, NULL, -1, NULL, -1, chosenHeal, i);
                    break;
                default:
                    printf("Invalid choice. A tenth of your cursed energy will be used to block (%d)\n", userTeam->cursedEnergy / 10);
                    chosenDefend = userTeam->cursedEnergy / 10;
                    cpuPlayGame(userTeam, cpuTeam, NULL, -1, NULL, chosenDefend, -1, i);
            }
        }
        usleep(DELAY);
        if ((userTeam->health <= 0 && cpuTeam->health <= 0) || (!canAttack(userTeam) && !canAttack(cpuTeam))) {
            printf("ITS A TIE!\n");
        } else if (userTeam->health > 0) {
            printf("%s WINS ROUND %d!\n\n", userTeam->roster[i]->name, i + 1);
            winCount++;
        } else if (cpuTeam->health > 0) {
            printf("%s WINS ROUND %d!\n\n", cpuTeam->roster[i]->name, i + 1);
            printf("You lost\n");
            lossCount++;
        }
    }

    if (winCount > lossCount) {
        printf("YOU WIN THIS BATTLE!\n");
        return;
    } else if (lossCount > winCount) {
        printf("YOU LOST THIS BATTLE! BETTER LUCK NEXT TIME\n");
        return;
    } else {
        printf("This battle was a tie!\n");
    }
}

void printPlayerAbilities(player *player) {
    for (int i = 0; i < NUM_ABILITIES; i++) {
        printf("%d. %s\nCursed Energy Consumption: %d\nDamage: %d\n", i + 1, player->arsenal[i]->abilityName, player->arsenal[i]->cursedEnergyConsumption, player->arsenal[i]->damage);
    }
    if (player->domain != NULL) {
        printf("4. %s (Domain Expansion)\n", player->domain->domainName);
        printf("Cursed Energy Consumption: %d\nDamage: %d\n", player->domain->cursedEnergyConsumption, player->domain->damage);
    }
}

void cpuPlayGame(team *userTeam, team *cpuTeam, abilities *userAbility, int userIdx, domainExpansion *userDomain, int userBlock, 
int userHeal, int round) {
    int poa;
    abilities *cpuAbility;
    domainExpansion *cpuDomain;
    int cpuBlock;
    int cpuHeal;
    srand(time(NULL));
    domainIndicator(&cpuDI);
    if (!canAttack(cpuTeam) || cpuTeam->cursedEnergy <= 0 || cpuDI != 0) {
        if (cpuTeam->cursedEnergy <= 0) {
            poa = 3;
        } else  if (!canAttack(cpuTeam) || cpuDI != 0) {
            poa = (rand() % 2) + 1;
        }
    } else {
        poa = rand() % 3;
    }
    switch(poa) {
        case 0:
            int cA;
            if (canUseAnyAbility(cpuTeam)) {
                if (cpuTeam->roster[round]->domain != NULL) {
                    cA = rand() % 4;
                } else {
                    cA = rand() % 3;
                }
                while (cpuTeam->roster[currRound]->abilityUsed[cA] != 0) {
                    if (cpuTeam->roster[round]->domain != NULL) {
                        cA = rand() % 4;
                    } else {
                        cA = rand() % 3;
                    }
                }    
            } else {
                cA = useThisAbility(cpuTeam);
            }
            switch(cA) {
                case 0:
                    cpuAbility = cpuTeam->roster[round]->arsenal[0];
                    battleGround(userTeam, cpuTeam, userAbility, userIdx, userDomain, userBlock, userHeal, cpuAbility, 0, NULL, -1, -1);
                    break;
                case 1:
                    cpuAbility = cpuTeam->roster[round]->arsenal[1];
                    battleGround(userTeam, cpuTeam, userAbility, userIdx, userDomain, userBlock, userHeal, cpuAbility, 1, NULL, -1, -1);
                    break; 
                case 2:
                    cpuAbility = cpuTeam->roster[round]->arsenal[2];
                    battleGround(userTeam, cpuTeam, userAbility, userIdx, userDomain, userBlock, userHeal, cpuAbility, 2, NULL, -1, -1);
                    break;
                case 3:
                    cpuDomain = cpuTeam->roster[round]->domain;
                    battleGround(userTeam, cpuTeam, userAbility, userIdx, userDomain, userBlock, userHeal, NULL, -1, cpuDomain, -1, -1);
                    break;
                default:
                    int r;
                    r = rand() % NUM_ABILITIES;
                    cpuAbility = cpuTeam->roster[round]->arsenal[r];
                    battleGround(userTeam, cpuTeam, userAbility, userIdx, userDomain, userBlock, userHeal, cpuAbility, r, NULL, -1, -1);
            }
            break;
        case 1:
            cpuBlock = ((rand() % cpuTeam->cursedEnergy) + 1) / 6;
            battleGround(userTeam, cpuTeam, userAbility, userIdx, userDomain, userBlock, userHeal, NULL, -1, NULL, cpuBlock, -1);
            break;
        case 2:
            cpuHeal = (rand() % cpuTeam->health) / 6;
            battleGround(userTeam, cpuTeam, userAbility, userIdx, userDomain, userBlock, userHeal, NULL, -1, NULL, -1, cpuHeal);
            break;
        default:
            printf("Executed default for cpu\n");
            if (cpuTeam->cursedEnergy <= 0) {
                battleGround(userTeam, cpuTeam, userAbility, userIdx, userDomain, userBlock, userHeal, NULL, -1, NULL, 0, -1);
            } else {
                cpuBlock = (rand() % cpuTeam->cursedEnergy) / 10;
                battleGround(userTeam, cpuTeam, userAbility, userIdx, userDomain, userBlock, userHeal, NULL, -1, NULL, cpuBlock, -1);
            }
    }    
}

void battleGround(team *userTeam, team *cpuTeam, abilities *userAbility, int userIdx, domainExpansion *userDomain, int userBlock, 
int userHeal, abilities *cpuAbility, int cpuIdx, domainExpansion *cpuDomain, int cpuBlock, int cpuHeal) {
    if (userAbility != NULL && cpuAbility != NULL) {
        bG1(userTeam, cpuTeam, userAbility, cpuAbility);
    } else if (userAbility != NULL && cpuBlock >= 0) {
        bG2(userTeam, cpuTeam, userAbility, cpuBlock);
    } else if (userAbility != NULL && cpuHeal >= 0) {
        bG3(userTeam, cpuTeam, userAbility, cpuHeal);
    } else if (userAbility != NULL && cpuDomain != NULL) {
        bG4(userTeam, cpuTeam, userAbility, cpuDomain);
    } else if (userBlock > 0 && cpuAbility != NULL) {
        bG5(userTeam, cpuTeam, userBlock, cpuAbility);
    } else if (userBlock > 0 && cpuBlock >= 0) {
        bG6(userTeam, cpuTeam, userBlock, cpuBlock);
    } else if (userBlock > 0 && cpuHeal >= 0) {
        bG7(userTeam, cpuTeam, userBlock, cpuHeal);
    } else if (userBlock > 0 && cpuDomain != NULL) {
        bG8(userTeam, cpuTeam, userBlock, cpuDomain);
    } else if (userHeal > 0 && cpuAbility != NULL) {
        bG9(userTeam, cpuTeam, userHeal, cpuAbility);
    } else if (userHeal > 0 && cpuBlock >= 0) {
        bG10(userTeam, cpuTeam, userHeal, cpuBlock);
    } else if (userHeal > 0 && cpuHeal >= 0) {
        bG11(userTeam, cpuTeam, userHeal, cpuHeal);
    } else if (userHeal > 0 && cpuDomain != NULL) {
        bG12(userTeam, cpuTeam, userHeal, cpuDomain);
    } else if (userDomain != NULL && cpuAbility != NULL) {
        bG13(userTeam, cpuTeam, userDomain, cpuAbility);
    } else if (userDomain != NULL && cpuBlock >= 0) {
        bG14(userTeam, cpuTeam, userDomain, cpuBlock);
    } else if (userDomain != NULL && cpuHeal >= 0) {
        bG15(userTeam, cpuTeam, userDomain, cpuHeal);
    } else if (userDomain != NULL && cpuDomain != NULL) {
        bG16(userTeam, cpuTeam, userDomain, cpuDomain);
    }
}

void bG1(team *userTeam, team *cpuTeam, abilities *userAbility, abilities *cpuAbility) {
    if (userTeam->cursedEnergy < userAbility->cursedEnergyConsumption) {
        printf("You do not have enough cursed energy for %s\n", userAbility->abilityName);
        usleep(SMALL_DELAY);
        printf("You will do nothing this turn\n");
        printf("bG1\n");
        usleep(DELAY);
        printf("%s is attacking with %s which does %d damage!\n", cpuTeam->roster[currRound]->name, cpuAbility->abilityName, cpuAbility->damage);
        userTeam->health -= cpuAbility->damage;
        cpuTeam->cursedEnergy -= cpuAbility->cursedEnergyConsumption;
        useAbility(cpuTeam->roster[currRound], cpuAbility);
        usleep(1000000);
    } else {
        printf("*You attack*\n");
        usleep(DELAY);
        printf("%s has also chosen to attack!\n", cpuTeam->roster[currRound]->name);
        usleep(SMALL_DELAY);
        printf("They are attacking with %s which does %d damage!\n", cpuAbility->abilityName, cpuAbility->damage);
        usleep(SMALL_DELAY);
        printf("*You both attack*\n");
        userTeam->health -= cpuAbility->damage;
        userTeam->cursedEnergy -= userAbility->cursedEnergyConsumption;
        cpuTeam->cursedEnergy -= cpuAbility->cursedEnergyConsumption;
        cpuTeam->health -= userAbility->damage;
        useAbility(userTeam->roster[currRound], userAbility);
        useAbility(cpuTeam->roster[currRound], cpuAbility);
        usleep(1000000);
    }
}

void bG2(team *userTeam, team *cpuTeam, abilities *userAbility, int cpuBlock) {
    if (userTeam->cursedEnergy < userAbility->cursedEnergyConsumption) {
        printf("You do not have enough cursed energy for %s\n", userAbility->abilityName);
        printf("You will do nothing this turn\n");
        usleep(DELAY);
        printf("%s is blocking but you did not attack\n", cpuTeam->roster[currRound]->name);
        cpuTeam->cursedEnergy -= cpuBlock;
        useAbility(userTeam->roster[currRound], userAbility);
    } else {
        printf("*You attack*\n");
        usleep(DELAY);
        printf("%s is trying to block your %s!\n", cpuTeam->roster[currRound]->name, userAbility->abilityName);
        usleep(DELAY);
        if (cpuBlock < userAbility->damage) {
            printf("They blocked %d of your %s's damage!\n", cpuBlock, userAbility->abilityName);
            printf("You managed to overpower %s's block and do %d damage!\n", cpuTeam->roster[currRound]->name, userAbility->damage - cpuBlock);
            userTeam->cursedEnergy -= userAbility->cursedEnergyConsumption;
            cpuTeam->cursedEnergy -= cpuBlock;
            cpuTeam->health -= (userAbility->damage - cpuBlock);
        } else {
            printf("%s completely blocked your attack!\n", cpuTeam->roster[currRound]->name);
            userTeam->cursedEnergy -= userAbility->cursedEnergyConsumption;
            cpuTeam->cursedEnergy -= cpuBlock;
        }
        useAbility(userTeam->roster[currRound], userAbility);
    }
    usleep(1000000);
}

void bG3(team *userTeam, team *cpuTeam, abilities *userAbility, int cpuHeal) {
    if (userTeam->cursedEnergy < userAbility->cursedEnergyConsumption) {
        printf("You do not have enough cursed energy for %s\n", userAbility->abilityName);
        printf("You will do nothing this turn\n");
        usleep(DELAY);
        printf("%s just healed %d health\n", cpuTeam->roster[currRound]->name, cpuHeal);
        cpuTeam->cursedEnergy -= cpuHeal;
        cpuTeam->health += cpuHeal;
        useAbility(userTeam->roster[currRound], userAbility);
    } else {
        printf("*You attack*\n");
        usleep(DELAY);
        printf("%s is trying to outheal your %s!\n", cpuTeam->roster[currRound]->name, userAbility->abilityName);
        usleep(DELAY);
        if (cpuHeal < userAbility->damage) {
            printf("You overpowered %s's heal and did %d damage!\n", cpuTeam->roster[currRound]->name, userAbility->damage - cpuHeal);
            userTeam->cursedEnergy -= userAbility->cursedEnergyConsumption;
            cpuTeam->cursedEnergy -= cpuHeal;
            cpuTeam->health -= (userAbility->damage - cpuHeal); 
        } else {
            printf("%s outhealed your %s!\n", cpuTeam->roster[currRound]->name, userAbility->abilityName);
            userTeam->cursedEnergy -= userAbility->cursedEnergyConsumption;
            cpuTeam->cursedEnergy -= cpuHeal;
            cpuTeam->health += cpuHeal - userAbility->damage;
        }
        useAbility(userTeam->roster[currRound], userAbility);
    }
    usleep(100000);
}

void bG4(team *userTeam, team *cpuTeam, abilities *userAbility, domainExpansion *cpuDomain) {
    if (userTeam->cursedEnergy < userAbility->cursedEnergyConsumption) {
        printf("You do not have enough cursed energy for %s\n", userAbility->abilityName);
        printf("You will do nothing this turn\n");
        printf("bG4\n");
        usleep(DELAY);
        char d[19] = "Domain Expansion: ";
        for (int i = 0; i < strlen(d); i++) {
            printf("%c", d[i]);
            usleep(10000);
        }
        usleep(DELAY);
        for (int i = 0; i < strlen(cpuDomain->domainName); i++) {
            printf("%c", cpuDomain->domainName[i]);
        }
        printf("\n");
        usleep(DELAY);
        readFromFile(cpuDomain->fileName);
        usleep(15000);
        printf("\n%s used their domain expansion!\n", cpuTeam->roster[currRound]->name);
        userTeam->health -= cpuDomain->damage;
        cpuTeam->cursedEnergy -= cpuDomain->cursedEnergyConsumption;
        domainWasUsed(cpuTeam->roster[currRound]);
        useAbility(userTeam->roster[currRound], userAbility);
    } else {
        printf("*You attack*\n");
        usleep(DELAY);
        char d[19] = "Domain Expansion: ";
        for (int i = 0; i < strlen(d); i++) {
            printf("%c", d[i]);
            usleep(10000);
        }
        usleep(DELAY);
        for (int i = 0; i < strlen(cpuDomain->domainName); i++) {
            printf("%c", cpuDomain->domainName[i]);
        }
        printf("\n");
        usleep(DELAY);
        readFromFile(cpuDomain->fileName);
        usleep(15000);
        printf("%s used their domain expansion!\n", cpuTeam->roster[currRound]->name);
        userTeam->cursedEnergy -= userAbility->cursedEnergyConsumption;
        userTeam->health -= cpuDomain->damage;
        cpuTeam->cursedEnergy -= cpuDomain->cursedEnergyConsumption;
        cpuTeam->health -= userAbility->damage;
        useAbility(userTeam->roster[currRound], userAbility);
        domainWasUsed(cpuTeam->roster[currRound]);
        cpuDI++;
    }
    usleep(DELAY);
}

void bG5(team *userTeam, team *cpuTeam, int userBlock, abilities *cpuAbility) {
    if (userBlock > userTeam->cursedEnergy) {
        printf("You cannot block with this much cursed energy\n");
        usleep(DELAY);
        printf("%s has taken advantage of this and attacked with their %s!\n", cpuTeam->roster[currRound]->name, cpuAbility->abilityName);
        userTeam->health -= cpuAbility->damage;
        cpuTeam->cursedEnergy -= cpuAbility->cursedEnergyConsumption;
        useAbility(cpuTeam->roster[currRound], cpuAbility);
    } else {
        printf("*You block*\n");
        usleep(DELAY);
        printf("%s is trying to overpower your block with their %s!\n", cpuTeam->roster[currRound]->name, cpuAbility->abilityName);
        usleep(DELAY);
        if (cpuAbility->damage > userBlock) {
            printf("%s overpowered your block! You were left vulnerable and took %d damage!\n", cpuTeam->roster[currRound]->name, cpuAbility->damage - userBlock);
            userTeam->health -= (cpuAbility->damage - userBlock);
        } else {
            printf("You completely blocked %s's attack!\n", cpuTeam->roster[currRound]);
        }
        userTeam->cursedEnergy -= userBlock;
        cpuTeam->cursedEnergy -= cpuAbility->cursedEnergyConsumption;
        useAbility(cpuTeam->roster[currRound], cpuAbility);
    }
    usleep(DELAY);
}

void bG6(team *userTeam, team *cpuTeam, int userBlock, int cpuBlock) {
    if (userBlock > userTeam->cursedEnergy) {
        printf("You cannot block with this much cursed energy\n");
        usleep(DELAY);
        printf("%s thought you were going to attack and blocked too\n", cpuTeam->roster[currRound]->name);
        cpuTeam->cursedEnergy -= cpuBlock;
    } else {
        printf("*You block*\n");
        usleep(DELAY);
        printf("%s thought you were going to attack and blocked too\n", cpuTeam->roster[currRound]->name);
        userTeam->cursedEnergy -= userBlock;
        cpuTeam->cursedEnergy -= cpuBlock;
    }
    usleep(DELAY);
}

void bG7(team *userTeam, team *cpuTeam, int userBlock, int cpuHeal) {
    if (userBlock > userTeam->cursedEnergy) {
        printf("You cannot block with this much cursed energy\n");
    } else {
        printf("*You block*\n");
        userTeam->cursedEnergy -= userBlock;
    }
    usleep(DELAY);
    printf("%s took advantage of this and healed!\n", cpuTeam->roster[currRound]->name);
    cpuTeam->health += cpuHeal;
    cpuTeam->cursedEnergy -= cpuHeal;
    usleep(DELAY);
}

void bG8(team *userTeam, team *cpuTeam, int userBlock, domainExpansion *cpuDomain) {
    if (userBlock > userTeam->cursedEnergy) {
        printf("You cannot block with this much cursed energy\n");
        userTeam->health -= cpuDomain->damage;
    } else {
        printf("*You block*\n");
        userTeam->cursedEnergy -= userBlock;
    }
    usleep(DELAY);
    char d[19] = "Domain Expansion: ";
    for (int i = 0; i < strlen(d); i++) {
        printf("%c", d[i]);
        usleep(10000);
    }
    usleep(DELAY);
    for (int i = 0; i < strlen(cpuDomain->domainName); i++) {
        printf("%c", cpuDomain->domainName[i]);
    }
    printf("\n");
    usleep(DELAY);
    readFromFile(cpuDomain->fileName);
    usleep(15000);
    printf("%s used their domain expansion!\n", cpuTeam->roster[currRound]->name);
    cpuDI++;
    domainWasUsed(cpuTeam->roster[currRound]);
    cpuTeam->cursedEnergy -= cpuDomain->cursedEnergyConsumption;
    if (userBlock <= userTeam->cursedEnergy && userBlock >= cpuDomain->damage) {
        usleep(SMALL_DELAY);
        printf("You managed to block all the damage from %s!!\n", cpuDomain->domainName);
    } else if (userBlock <= userTeam->cursedEnergy && userBlock < cpuDomain->damage) {
        userTeam->health -= (cpuDomain->damage - userBlock);
    }
    usleep(DELAY);
}

void bG9(team *userTeam, team *cpuTeam, int userHeal, abilities *cpuAbility) {
    if (userHeal > (userTeam->cursedEnergy / 2)) {
        printf("You cannot heal with this much cursed energy\n");
        usleep(DELAY);
        printf("%s has taken advantage of this and attacked with their %s!\n", cpuTeam->roster[currRound]->name, cpuAbility->abilityName);
        userTeam->health -= cpuAbility->damage;
        cpuTeam->cursedEnergy -= cpuAbility->cursedEnergyConsumption;
    } else {
        printf("You plan to heal and hope %s doesn't realize...\n", cpuTeam->roster[currRound]->name);
        usleep(DELAY);
        printf("%s noticed this and attacked with %s!\n", cpuTeam->roster[currRound]->name, cpuAbility->abilityName);
        usleep(DELAY);
        if (userHeal >= cpuAbility->damage) {
            printf("You managed to heal back the damage from %s's %s\n", cpuTeam->roster[currRound]->name, cpuAbility->abilityName);
            usleep(SMALL_DELAY);
            printf("You healed %d health\n", (userHeal - cpuAbility->damage));
            userTeam->cursedEnergy -= userHeal;
            userTeam->health += (userHeal - cpuAbility->damage);
            cpuTeam->cursedEnergy -= cpuAbility->cursedEnergyConsumption;
        } else {
            printf("%s's %s did more damage than you could heal!\n", cpuTeam->roster[currRound]->name, cpuAbility->abilityName);
            usleep(SMALL_DELAY);
            printf("You took %d damage from that attack\n", (cpuAbility->damage - userHeal));
            userTeam->cursedEnergy -= userHeal;
            userTeam->health -= (cpuAbility->damage - userHeal);
            cpuTeam->cursedEnergy -= cpuAbility->cursedEnergyConsumption;
        }
    }
    useAbility(cpuTeam->roster[currRound], cpuAbility);
    usleep(DELAY);
}

void bG10(team *userTeam, team *cpuTeam, int userHeal, int cpuBlock) {
    if (userHeal > (userTeam->cursedEnergy / 2)) {
        printf("You cannot heal with this much cursed energy\n");
        usleep(DELAY);
        printf("Luckily %s thought you would attack and decided to block\n", cpuTeam->roster[currRound]->name);
        cpuTeam->cursedEnergy -= cpuBlock;
    } else {
        printf("You plan to heal and hope %s doesn't realize...\n", cpuTeam->roster[currRound]->name);
        usleep(DELAY);
        printf("%s thought you attack and blocked! You managed to heal!\n", cpuTeam->roster[currRound]->name);
        userTeam->cursedEnergy -= userHeal;
        userTeam->health += userHeal;
        cpuTeam->cursedEnergy -= cpuBlock;
    }
    usleep(DELAY);
}

void bG11(team *userTeam, team *cpuTeam, int userHeal, int cpuHeal) {
    if (userHeal > (userTeam->cursedEnergy / 2)) {
        printf("You cannot heal with this much cursed energy\n");
        usleep(DELAY);
        printf("%s took advantage of this and healed %d health!\n", cpuTeam->roster[currRound]->name, cpuHeal);
    } else {
        printf("You plan to heal and hope %s doesn't realize...\n", cpuTeam->roster[currRound]->name);
        usleep(DELAY);
        printf("%s took the chance to heal too!\n", cpuTeam->roster[currRound]->name);
        userTeam->cursedEnergy -= userHeal;
        userTeam->health += userHeal;
    }
    cpuTeam->cursedEnergy -= cpuHeal;
    cpuTeam->health += cpuHeal;
    usleep(DELAY);
}

void bG12(team *userTeam, team *cpuTeam, int userHeal, domainExpansion *cpuDomain) {
    if (userHeal > (userTeam->cursedEnergy / 2)) {
        printf("You cannot heal with this much cursed energy\n");
        userTeam->health -= cpuDomain->damage;
    } else {
        printf("You plan to heal and hope %s doesn't realize...\n", cpuTeam->roster[currRound]->name);
        userTeam->cursedEnergy -= userHeal;
    }
    usleep(DELAY);
    char d[19] = "Domain Expansion: ";
    for (int i = 0; i < strlen(d); i++) {
        printf("%c", d[i]);
        usleep(10000);
    }
    usleep(DELAY);
    for (int i = 0; i < strlen(cpuDomain->domainName); i++) {
        printf("%c", cpuDomain->domainName[i]);
    }
    printf("\n");
    usleep(DELAY);
    readFromFile(cpuDomain->fileName);
    usleep(15000);
    printf("%s used their domain expansion!\n", cpuTeam->roster[currRound]->name);
    cpuDI++;
    domainWasUsed(cpuTeam->roster[currRound]);
    cpuTeam->cursedEnergy -= cpuDomain->cursedEnergyConsumption;
    if (userHeal <= (userTeam->cursedEnergy / 2) && userHeal >= cpuDomain->damage) {
        usleep(SMALL_DELAY);
        printf("You managed to heal back the damage from %s!!\n", cpuDomain->domainName);
    } else if (userHeal <= (userTeam->cursedEnergy / 2) && userHeal < cpuDomain->damage) {
        userTeam->health -= (cpuDomain->damage - userHeal);
    }
    usleep(DELAY);
}

void bG13(team *userTeam, team *cpuTeam, domainExpansion *userDomain, abilities *cpuAbility) {
    if (userDomain->cursedEnergyConsumption > userTeam->cursedEnergy) {
        printf("You do not have enough cursed energy for %s\n", userDomain->domainName);
        printf("You will do nothing this turn\n");
        usleep(DELAY);
        printf("%s has taken advantage of this and attacked with their %s!\n", cpuTeam->roster[currRound]->name, cpuAbility->abilityName);
    } else {
        executeDomain(userTeam, userDomain);
        dI++;
        usleep(DELAY);
        printf("%s managed to hit you with their %s before you could use your domain and did %d damage\n", cpuTeam->roster[currRound]->name, cpuAbility->abilityName, cpuAbility->damage);
        cpuTeam->health -= userDomain->damage;
    }
    userTeam->health -= cpuAbility->damage;
    cpuTeam->cursedEnergy -= cpuAbility->cursedEnergyConsumption;
    useAbility(cpuTeam->roster[currRound], cpuAbility);
    usleep(DELAY);
}

void bG14(team *userTeam, team *cpuTeam, domainExpansion *userDomain, int cpuBlock) {
    if (userDomain->cursedEnergyConsumption > userTeam->cursedEnergy) {
        printf("You do not have enough cursed energy for %s\n", userDomain->domainName);
        printf("You will do nothing this turn\n");
        usleep(DELAY);
        printf("Luckily %s thought you would attack and decided to block\n", cpuTeam->roster[currRound]->name);
    } else {
        executeDomain(userTeam, userDomain);
        dI++;
        usleep(DELAY);
        if (cpuBlock >= userDomain->damage) {
            printf("%s blocked all the damage from your %s!!\n", cpuTeam->roster[currRound]->name, userDomain->domainName);
        } else {
            printf("%s tried blocking your domain and blocked %d damage\n", cpuTeam->roster[currRound]->name, cpuBlock);
            cpuTeam->health -= (userDomain->damage - cpuBlock);
        }
    }
    cpuTeam->cursedEnergy -= cpuBlock;
    usleep(DELAY);
}

void bG15(team *userTeam, team *cpuTeam, domainExpansion *userDomain, int cpuHeal) {
    if (userDomain->cursedEnergyConsumption > userTeam->cursedEnergy) {
        printf("You do not have enough cursed energy for %s\n", userDomain->domainName);
        printf("You will do nothing this turn\n");
        usleep(DELAY);
    } else {
        executeDomain(userTeam, userDomain);
        dI++;
        usleep(DELAY);
        printf("%s needed to heal after that attack and healed %d health\n", cpuTeam->roster[currRound]->name, cpuHeal);
        cpuTeam->health -= (userDomain->damage - cpuHeal);
    }
    cpuTeam->cursedEnergy -= cpuHeal;
    usleep(DELAY);
}

void bG16(team *userTeam, team *cpuTeam, domainExpansion *userDomain, domainExpansion *cpuDomain) {
    if (userDomain->cursedEnergyConsumption > userTeam->cursedEnergy) {
        printf("You do not have enough cursed energy for %s\n", userDomain->domainName);
        printf("You will do nothing this turn\n");
        usleep(DELAY);
    } else {
        executeDomain(userTeam, userDomain);
        dI++;
        usleep(DELAY);
        executeDomain(cpuTeam, cpuDomain);
        cpuDI++;
        printf("DOMAIN CLASH!!!\n");
        usleep(SMALL_DELAY);
        printf("%s USED THEIR DOMAIN WHILE INSIDE YOUR %s!\n", cpuTeam->roster[currRound]->name, userDomain->domainName);
        cpuTeam->health -= userDomain->damage;
    }
    userTeam->health -= cpuDomain->damage;
    usleep(DELAY);
}

void resetAbilities(player *p) {
    for (int i = 0; i < NUM_ABILITIES + 1; i++) {
        p->abilityUsed[i] = 0;
    }
}

void useAbility(player *p, abilities *a) {
    if (a == NULL) {
        return;
    }
    for (int i = 0; i < NUM_ABILITIES; i++) {
        if (strcmp(a->abilityName, p->arsenal[i]->abilityName) == 0) {
            p->abilityUsed[i] = 1;
            break;
        }
    }
}

bool checkAbility(player *p, abilities *a) {
    if (a == NULL) {
        return NULL;
    }
    int i;
    for (i = 0; i < NUM_ABILITIES + 1; i++) {
        if (strcmp(a->abilityName, p->arsenal[i]->abilityName) == 0) {
            return (p->abilityUsed[i] == 0);
        }
    }
}

bool allUsed(player *p) {
    if (p->domain != NULL) {
        for (int i = 0; i < NUM_ABILITIES + 1; i++) {
            if (p->abilityUsed[i] == 0) {
                return false;
            }
        }
    } else {
        for (int i = 0; i < NUM_ABILITIES; i++) {
            if (p->abilityUsed[i] == 0) {
                return false;
            }
        }
    }   
    return true;
}

void useAll(player *p) {
    for (int i = 0; i < NUM_ABILITIES + 1; i++) {
        p->abilityUsed[i] = 1;
    }
}

void domainIndicator(int *d) {
    if (*d != 0) {
        (*d)++;
    }
    if (*d >= 3) {
        *d = 0;
    }
}

void domainWasUsed(player *p) {
    p->abilityUsed[3] = 1;
}

bool canAttack(team *t) {
    if (allUsed(t->roster[currRound])) {
        resetAbilities(t->roster[currRound]);
    }
    int min = 999999;
    if (t->roster[currRound]->domain == NULL) {
        for (int i = 0; i < NUM_ABILITIES; i++) {
            if (t->roster[currRound]->arsenal[i]->cursedEnergyConsumption < min && t->roster[currRound]->abilityUsed[i] == 0) {
                min = t->roster[currRound]->arsenal[i]->cursedEnergyConsumption;
            }
        }
    } else {
        for (int i = 0; i < NUM_ABILITIES + 1; i++) {
            if (t->roster[currRound]->arsenal[i]->cursedEnergyConsumption < min && t->roster[currRound]->abilityUsed[i] == 0) {
                min = t->roster[currRound]->arsenal[i]->cursedEnergyConsumption;
            }
        }
    }
    return t->cursedEnergy >= min;
}

bool canUseAnyAbility(team *t) {
    if (t->roster[currRound]->domain != NULL) {
        return t->cursedEnergy >= t->roster[currRound]->domain->cursedEnergyConsumption;
    } else {
        for (int i = 0; i < NUM_ABILITIES; i++) {
            if (t->cursedEnergy < t->roster[currRound]->arsenal[i]->cursedEnergyConsumption) {
                return false;
            }
        }
        return true;
    }
}

int useThisAbility(team *t) {
    if (t->roster[currRound]->domain != NULL) {
        for (int i = 0; i < NUM_ABILITIES + 1; i++) {
            if (t->roster[currRound]->arsenal[i]->cursedEnergyConsumption <= t->cursedEnergy 
                && t->roster[currRound]->abilityUsed[i] == 0) {
                return i;
            }
        }
    } else {
        for (int i = 0; i < NUM_ABILITIES; i++) {
            if (t->roster[currRound]->arsenal[i]->cursedEnergyConsumption <= t->cursedEnergy 
                && t->roster[currRound]->abilityUsed[i] == 0) {
                return i;
            }
        }
    }
}

void executeDomain(team *t, domainExpansion *dE) {
    usleep(DELAY);
    char d[19] = "Domain Expansion: ";
    for (int i = 0; i < strlen(d); i++) {
        printf("%c", d[i]);
        usleep(10000);
    }
    usleep(DELAY);
    for (int i = 0; i < strlen(dE->domainName); i++) {
        printf("%c", dE->domainName[i]);
    }
    printf("\n");
    usleep(DELAY);
    readFromFile(dE->fileName);
    usleep(SMALL_DELAY);
    printf("\n%s used their domain expansion!\n", t->roster[currRound]->name);
    domainWasUsed(t->roster[currRound]);
    t->cursedEnergy -= dE->cursedEnergyConsumption;
}