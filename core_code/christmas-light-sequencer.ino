#include "sequences.h"

/* Hardware */
const unsigned int MIN_DELAY = 200;
const int NUM_RELAYS = 4;
int r1 = D0;
int r2 = D1;
int r3 = D2;
int r4 = D3;
int relays[] = {r1, r2, r3, r4};

/*Status*/
byte current_vector = 0x0;
bool sequencing = 0;
unsigned long step_delay = 0;

byte *current_sequence;
int sequence_length;
int step = 0;
unsigned long last_stepped = 0;

/* Output */
char msg[64];


void setup() {
    pinMode(r1, OUTPUT);
    pinMode(r2, OUTPUT);
    pinMode(r3, OUTPUT);
    pinMode(r4, OUTPUT);

    Spark.function("toggle", toggle_relays);
    Spark.function("off", open_relays);
    Spark.function("on", close_relays);
    Spark.function("sequence", sequence);

    Spark.variable("state", &current_vector, INT);
    Spark.variable("sequencing", &sequencing, INT);
    Spark.variable("step_delay", &step_delay, INT);
}

void loop() {
    if (sequencing) {
        unsigned long current_millis = millis();
        if (current_millis - last_stepped > step_delay) {
            ++step;
            step %= sequence_length;

            current_vector = current_sequence[step];
            write_vector();

            last_stepped = current_millis;
        }
    }
}

/* EXPOSED FUNCTIONS */
int sequence(String command) {
    String parsed[3];
    for (int i = 0; i < 3; ++i) {
        parsed[i].reserve(64);
    }
    tokenize_command(command, parsed, 3);

    String new_sequence = parsed[0];
    unsigned long new_delay = parsed[1].toInt();
    unsigned long length = parsed[2].toInt();

    if (new_delay < MIN_DELAY)
        swap(new_delay, length);

    if (new_sequence.equals("cylon")) {
        switch(length) {
            case 2:
                current_sequence = cylon2;
                sequence_length = cylon2_len;
                break;
            case 3:
                current_sequence = cylon3;
                sequence_length = cylon3_len;
                break;
            default:
                current_sequence = cylon1;
                sequence_length = cylon1_len;
                break;
        };
    } else if (new_sequence.equals("chase")) {
        switch(length) {
            case 2:
                current_sequence = chase2;
                sequence_length = chase2_len;
                break;
            case 3:
                current_sequence = chase3;
                sequence_length = chase3_len;
                break;
            default:
                current_sequence = chase1;
                sequence_length = chase1_len;
                break;
        };
    } else if (new_sequence.equals("flap")) {
        current_sequence = flap;
        sequence_length = flap_len;
    } else if (new_sequence.equals("walk")) {
        current_sequence = walk;
        sequence_length = walk_len;

    } else {
        return -1;
    }

    step_delay = new_delay < MIN_DELAY ? 1000 : new_delay;
    sequencing = 1;
    step = 0;

    char tmp[15];
    new_sequence.toCharArray(tmp, 15);
    sprintf(msg, "starting %s sequence with length %u and delay %u",
        tmp, length, step_delay);
    Spark.publish("sequence-started", msg);

    current_vector = current_sequence[step];
    write_vector();
    last_stepped = millis();
    return 0;
}

int toggle_relays(String command) { return change_relays(command, 't'); }
int open_relays(String command)   { return change_relays(command, 'o'); }
int close_relays(String command)  { return change_relays(command, 'c'); }

/* HELPER FUNCTIONS */
/*
TOGGLING:
    current 0b1111    current 0b0000    current 0b1100
    toggle  0b1010    toggle  0b1010    toggle  0b1010
    xor     0b0101    xor     0b1010    xor     0b0110
    target  0b0101    target  0b1010    target  0b0110

OPENING:
    current 0b1111    current 0b0000    current 0b1100
    to_open 0b1010    to_open 0b1010    to_open 0b1010
    flipped 0b0101    flipped 0b0101    flipped 0b0101
    and     0b0101    and     0b0000    and     0b0100
    target  0b0101    target  0b0000    target  0b0100

CLOSING:
    current  0b1111    current  0b0000    current  0b1100
    to_close 0b1010    to_close 0b1010    to_close 0b1010
    or       0b1010    or       0b1010    or       0b1110
    target   0b1010    target   0b1010    target   0b1110
*/
byte change_relays(String command, char oper) {
    byte affected_relays = 0xf;

    if (command.length() > 0) {
        String parsed[NUM_RELAYS];
        for (int i = 0; i < NUM_RELAYS; ++i) {
            parsed[i].reserve(64);
        }

        tokenize_command(command, parsed, NUM_RELAYS);
        affected_relays = input_to_vector(parsed, NUM_RELAYS);
    }

    char operation[16];
    switch (oper) {
        case 't':
            strncpy(operation, "toggling:   ", 16);
            current_vector ^= affected_relays;
            break;
        case 'o':
            strncpy(operation, "turning off:", 16);
            current_vector &= affected_relays ^ 0xf;
            break;
        case 'c':
            strncpy(operation, "turning on: ", 16);
            current_vector |= affected_relays;
            break;
    }

    char affected_relays_str[8];
    char current_vector_str[8];
    print_vector(affected_relays, affected_relays_str);
    print_vector(current_vector, current_vector_str);
    sprintf(msg, "%s %s  new state: %s",
        operation, affected_relays_str, current_vector_str);
    Spark.publish("lights-changed", msg);

    sequencing = 0;
    write_vector();
    return current_vector;
}

void tokenize_command(String command, String parts[], int max_parts) {
    int n = 0;
    unsigned int j = 0;
    for (unsigned int i = 0; i < command.length(); ++i) {
        if (n >= max_parts) {  // Bounds checking; don't overrun the parts array
            Spark.publish("debug", "reached part limit before end of cmd", 0, PRIVATE);
            break;
        }

        // spaces and tabs are token delimiters; skip them
        if (command[i] == ' ' || command[i] == ' ') {
            ++n;
            j = 0;
            continue;
        }

        // Copy this character into the appropriate part.
        parts[n] += command[i];

        if (j >= parts[j].length()) {
            j = 0;
            ++n;
            sprintf(msg, "overran part limit: i=%d", i);
            Spark.publish("debug", msg, 0, PRIVATE);
        }
    }
    return;
}

byte input_to_vector(String parts[], int max_parts) {
    byte bitfield = 0;
    for(int i = 0; i < max_parts; ++i) {
        int relay = parts[i].toInt() - 1;
        if (0 <= relay && relay <= 3)
            bitfield |= (0x1 << relay);
    }
    return bitfield;
}

void write_vector() {
    for (int i = 0; i < NUM_RELAYS; ++i)
        digitalWrite(relays[i], current_vector & 0x1 << i);
}

void swap(unsigned long &a, unsigned long &b) {
    unsigned long tmp = a;
    a = b;
    b = tmp;
}

void print_vector(byte vector, char output[8]) {
    int one   = (vector & 0b0001) >> 0;
    int two   = (vector & 0b0010) >> 1;
    int three = (vector & 0b0100) >> 2;
    int four  = (vector & 0b1000) >> 3;
    sprintf(output, "%d %d %d %d", four, three, two, one);
}

