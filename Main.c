#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define DECK_SIZE 108
#define _CRT_SECURE_NO_WARNINGS

typedef struct card_s {
    char color[10];
    int value;
    char action[15];
    struct card_s* pt;
} card;

// Define players' hand structure
typedef struct node_s {
    card* c;
    struct node_s* next;
} node;

// Define center line
typedef struct centerrow_s {
    int numcards;
    card* head;
    card* tail;
} centerrow;

void print_card(card* c) {
    printf("%s ", c->color);
    if (c->value > 0) {
        printf("%d ", c->value);
    }
    else {
        printf("%s ", c->action);
    }
}

void print_list(card* head) {
    node* curr = head;
    while (curr != NULL) {
        printf(" ");
        print_card(curr->c);
        curr = curr->next;
    }
    printf("\n");
}

card* draw_card(card* deck[], int* deck_size) {
    if (*deck_size <= 0) {
        return NULL;
    }
    int idx = rand() % *deck_size;
    card* c = deck[idx];
    deck[idx] = deck[*deck_size - 1];
    deck[*deck_size - 1] = NULL;
    (*deck_size)--;
    return c;
}

void shuffle(card* deck) {
    srand(time(NULL));
    for (int i = DECK_SIZE - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

// Create the players' hands
void dealCards(card** p1_hand, card** p2_hand, card* deck) {
    int i;
    // Deal cards to players
    for (i = 0; i < 7; i++) {
        p1_hand[i] = &deck[i + 7];
        p2_hand[i] = &deck[i + 21];
    }
}

// Add card to players hands
void add_to_player_hand(card* c, card* player_hand[], int* player_hand_size) {
    player_hand[*player_hand_size] = c;
    (*player_hand_size)++;
}

// Remove card from players hands
void remove_from_player_hand(card* player_hand[], int* player_hand_size, int index) {
    for (int i = index; i < *player_hand_size - 1; i++) {
        player_hand[i] = player_hand[i + 1];
    }
    (*player_hand_size)--;
}


// Add card to center row
void add_to_center_row(card* c, centerrow* center) {
    c->pt = NULL;
    if (center->numcards == 0) {
        center->head = c;
        center->tail = c;
    } else {
        center->tail->pt = c;
        center->tail = c;
    }
    center->numcards++;
}


// Remove card from center row
void remove_from_center_row(centerrow* center) {
    center->numcards--;
    if (center->numcards == 0) {
        center->head = NULL;
        center->tail = NULL;
    }
}

int main(void) {
    // Initialize the game
    FILE* fp;
    char line[108];
    int count = 0;
    centerrow center;
    int deck_size = DECK_SIZE;
    card* p1_hand[7], *p2_hand[7];
    int p1_hand_size = 0, p2_hand_size = 0;
    int current_player = 1;
    int turn_count = 0;
    card* deck = malloc(sizeof(card) * DECK_SIZE);
    
    // Open the file
    fp = fopen("deck.txt", "r");
    if (fp == NULL) {
        printf("Sorry I failed to open your .txt file.\n");
        return 1;
    }
    else{
        int shuffleornot = 0;
        
        printf("Let's Play a Game of DOS!!\n");
        printf("Press 1 to shuffle the DOS deck or 2 to load a deck from a file:");
        scanf("%d", &shuffleornot);
        printf("\n");
        
        if (shuffleornot == 1) {
            printf("\nThe deck has been shuffled!!\n");
            shuffle(deck);
        }
        else{
            printf("\nThe deck was not shuffled!!\n");
        }
    }
    
    // Read the contents of the file and initialize the deck
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = 0; // Remove newline characters
        sscanf(line, "%s %d %[^\n]", deck[count].color, &deck[count].value, deck[count].action);
        count++;
    }
    shuffle(deck);
    dealCards(p1_hand,p2_hand,deck);
    
    // Start the game loop
    while (1) {
        // Display the center line and the hand of the current player
        printf("Center: ");
        print_list(centerrow.head);
        printf("Player %d Hand: ", current_player);
        print_list(current_player == 1 ? p1_hand[0] : p2_hand[0]);
        
        int choice = 0;
        printf("Player %d, what would you like to do? (1) Play one card (2) Draw one card: ", current_player);
        scanf("%d", &choice);
        printf("\n");
        
        if (choice == 1) {
            int index = 0;
            printf("Enter the index of the card you want to play: ");
            scanf("%d", &index);
            if (index < 0 || index >= (current_player == 1 ? p1_hand_size : p2_hand_size)) {
                printf("Invalid index!\n");
                continue;
            }
            card* c = current_player == 1 ? p1_hand[index] : p2_hand[index];
            if (center.numcards == 0 || strcmp(c->color, center.tail->color) == 0 || c->value == center.tail->value) {
                // Play the card
                remove_from_player_hand(current_player == 1 ? p1_hand : p2_hand, current_player == 1 ? &p1_hand_size : &p2_hand_size, index);
                add_to_center_row(c, &center);
                printf("Player %d played: ", current_player);
                print_card(c);
                printf("\n");
            }
            if (strcmp(c->action, "draw2") == 0) {
                // Draw 2 cards from the deck and add them to the opponent's hand
                for (int i = 0; i < 2; i++) {
                    card* drawn_card = draw_card(deck, &deck_size);
                    if (drawn_card != NULL) {
                        add_to_player_hand(drawn_card, current_player == 1 ? p2_hand : p1_hand, current_player == 1 ? &p2_hand_size : &p1_hand_size);
                    }
                }
                printf("Player %d drew 2 cards!\n", current_player == 1 ? 2 : 1);
            } else if (strcmp(c->action, "reverse") == 0) {
                // Reverse the order of play
                printf("Order of play has been reversed!\n");
                current_player = current_player == 1 ? 2 : 1;
            } else if (strcmp(c->action, "skip") == 0) {
                // Skip the next player's turn
                printf("Next player's turn has been skipped!\n");
                current_player = current_player == 1 ? 2 : 1;
            }
        }
                    // Check if the player has won
                    if (current_player == 1 && p1_hand_size == 0) {
                        printf("Player 1 has won!\n");
                        break;
                    } else if (current_player == 2 && p2_hand_size == 0) {
                        printf("Player 2 has won!\n");
                        break;
                    }
                else {
                    printf("Invalid card!\n");
                    continue;
                }
            } else if (choice == 2) {
                // Play two cards
                int card_index1, card_index2;
                printf("Which cards would you like to play? (Enter indexes, separated by a space): ");
                scanf("%d %d", &card_index1, &card_index2);
                card *c1 = (current_player == 1 ? p1_hand[card_index1] : p2_hand[card_index1]);
                card *c2 = (current_player == 1 ? p1_hand[card_index2] : p2_hand[card_index2]);
                if (validate_two_cards(c1, c2, center_row)) {
                    // Remove the cards from the player's hand and add them to the center row
                    if (current_player == 1) {
                        remove_from_player_hand(p1_hand, &p1_hand_size);
                        remove_from_player_hand(p1_hand, &p1_hand_size);
                        add_to_center_row(c1, center_row, &center_row_size);
                        add_to_center_row(c2, center_row, &center_row_size);
                    } else {
                        remove_from_player_hand(p2_hand, &p2_hand_size, card_index1);
                        remove_from_player_hand(p2_hand, &p2_hand_size, card_index2);
                        add_to_center_row(c1, center_row, &center_row_size);
                        add_to_center_row(c2, center_row, &center_row_size);
                    }
                    printf("Cards played successfully!\n");
                    if (center_row_size >= 3) {
                        // Check for matching numbers on center row and apply appropriate actions
                        apply_center_row_rules(center_row, &center_row_size, &draw_count);
                    }
                }
            } else {
                printf("Invalid move. Please try again.\n");
            }
            else if (choice == 3) {
                // Draw a card from the deck
                card *c = draw_card(deck, &deck_size);
                if (c != NULL) {
                    if (current_player == 1) {
                        p1_hand[p1_hand_size] = c;
                        p1_hand_size++;
                    } else {
                        p2_hand[p2_hand_size] = c;
                        p2_hand_size++;
                    }
                    printf("Card drawn successfully!\n");
                } else {
                    printf("The deck is empty!\n");
                }
            }
            // Alternate turns between players
            current_player = (current_player == 1 ? 2 : 1);
    
    // End of game
    printf("Game over!\n");
    return 0;
}
   