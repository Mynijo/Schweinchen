#include <stdlib.h>
#include <time.h>

#include <algorithm>
#include <iostream>
#include <thread>
#include <vector>

using namespace std;

#define log_it false


class Player
{
   public:
    string player_name             = "Test";
    int points                     = 0;
    int total_wins                 = 0;
    int total_played               = 0;
    int player_one                 = 0;
    int rolled1                    = 0;
    bool was_last_time_fist_player = false;


    virtual bool play(vector<int> rolls, int points_other_player);
    bool won();
    void add_points(vector<int> rolls);
    void new_game()
    {
        points = 0;
        ++total_played;
    }
    void clear()
    {
        points                    = 0;
        total_wins                = 0;
        player_one                = 0;
        rolled1                   = 0;
        total_played              = 0;
        was_last_time_fist_player = false;
    }

   protected:
    int sum_vector(vector<int> v);
};

class KiMynijo : public Player
{
   public:
    bool play(vector<int> rolls, int points_other_player) override;

    KiMynijo()
    {
        player_name = "Mynijo";
    }
    void mutate(int mutate_value)
    {
        same_level_range += rand() % mutate_value * 2 - mutate_value;
        losing_min_value += rand() % mutate_value * 2 - mutate_value;
        draw_min_value += rand() % mutate_value * 2 - mutate_value;
        winning_min_value += rand() % mutate_value * 2 - mutate_value;
        panic_value += rand() % mutate_value * 2 - mutate_value;
        greed_for_win += rand() % mutate_value * 2 - mutate_value;

        if (greed_for_win <= 0) greed_for_win = 0;
        if (panic_value <= 0) panic_value = 0;
    }
    KiMynijo clone()
    {
        KiMynijo clone          = KiMynijo();
        clone.same_level_range  = same_level_range;
        clone.losing_min_value  = losing_min_value;
        clone.draw_min_value    = draw_min_value;
        clone.winning_min_value = winning_min_value;
        clone.panic_value       = panic_value;
        clone.greed_for_win     = greed_for_win;
        return clone;
    }

    void cout_stats()
    {
        cout << "Total winns " << total_wins << " Winrate: " << (100.0 / total_played) * total_wins << "% "
             << "Total played " << total_played << " same_level_range " << same_level_range << " losing_min_value "
             << losing_min_value << " draw_min_value " << draw_min_value << " winning_min_value " << winning_min_value
             << " panic_value " << panic_value << " greed_for_win " << greed_for_win;
    }

   private:
    int same_level_range = 0;

    int losing_min_value  = 25;
    int draw_min_value    = 18;
    int winning_min_value = 16;

    int panic_value   = 19;
    int greed_for_win = 5;
};


class KiMichael : public Player
{
   public:
    bool play(vector<int> rolls, int points_other_player) override;

    KiMichael()
    {
        player_name = "Michael";
    }
};

void switch_player(Player** curr_player, Player** other_player)
{
    Player* temp_player;
    temp_player   = *curr_player;
    *curr_player  = *other_player;
    *other_player = temp_player;
}

void play_the_game(KiMynijo* player1, KiMichael* player2)
{
    Player* curr_player  = player1;
    Player* other_player = player2;
    vector<int> rolls;
    curr_player->new_game();
    other_player->new_game();

    if (curr_player->was_last_time_fist_player)
    {
        switch_player(&curr_player, &other_player);
    }
    curr_player->was_last_time_fist_player  = true;
    other_player->was_last_time_fist_player = false;
    ++curr_player->player_one;

    while (true)
    {
        rolls.clear();
        while (true)
        {
            if (curr_player->play(rolls, other_player->points))
            {
                int rand_num = 0;
                rand_num     = rand() % 6 + 1;
                if (rand_num != 1)
                    rolls.push_back(rand_num);
                else
                {
                    ++curr_player->rolled1;
                    break;
                }
            }
            else
            {
                curr_player->add_points(rolls);
                break;
            }
        }
        if (curr_player->won()) break;
        switch_player(&curr_player, &other_player);
    }
    ++curr_player->total_wins;
}

bool Player::won()
{
    return (points >= 100);
}

void Player::add_points(vector<int> rolls)
{
    points += sum_vector(rolls);
}

int Player::sum_vector(vector<int> v)
{
    int value = 0;
    for (int i = 0; i < v.size(); ++i)
    {
        value += v[i];
    }
    return value;
}


void figth(KiMynijo* ki, int rounds)
{
    KiMichael KiMi = KiMichael();
    for (int i = 0; i < rounds; ++i)
        play_the_game(ki, &KiMi);
}

KiMynijo evolve()
{
    KiMynijo Last_winner_KiMy = KiMynijo();
    vector<KiMynijo> clones;
    vector<thread> ths;
    int clone_counter  = 10;
    int max_gereration = 10;
    int mutate_value   = 2;
    int rounds         = 10000;
    for (int gereration = 0; gereration < max_gereration; ++gereration)
    {
        Last_winner_KiMy.clear();
        clones.clear();
        ths.clear();

        for (int i = 0; i < clone_counter; ++i)
            clones.push_back(Last_winner_KiMy.clone());

        for (int i = 0; i < clones.size(); ++i)
            clones[i].mutate(mutate_value);

        figth(&Last_winner_KiMy, rounds);

        for (int i = 0; i < clones.size(); ++i)
            ths.push_back(thread(figth, &clones[i], rounds));

        for (int i = 0; i < ths.size(); ++i)
            ths[i].join();
        // figth(&clones[i], rounds);


        for (int i = 0; i < clones.size(); ++i)
            if (Last_winner_KiMy.total_wins < clones[i].total_wins) Last_winner_KiMy = clones[i];

        cout << "Winner of generation: " << gereration + 1 << "/" << max_gereration << " ";
        Last_winner_KiMy.cout_stats();
        cout << endl;
    }
    return Last_winner_KiMy;
}


int main()
{
    srand((unsigned) time(NULL));
    KiMynijo KiMy  = evolve();
    KiMichael KiMi = KiMichael();

    KiMy.clear();
    if (!log_it)
        for (int i = 0; i < 10000; ++i)
            play_the_game(&KiMy, &KiMi);
    else
        play_the_game(&KiMy, &KiMi);

    cout << KiMy.player_name << " Won: " << KiMy.total_wins << " Starte times: " << KiMy.player_one
         << " Rolled1: " << KiMy.rolled1 << endl;
    cout << KiMi.player_name << " Won: " << KiMi.total_wins << " Starte times: " << KiMi.player_one
         << " Rolled1: " << KiMi.rolled1 << endl;

    cout << KiMy.player_name << " ";
    KiMy.cout_stats();
    cout << endl;

    return 0;
}

bool Player::play(vector<int> rolls, int points_other_player)
{
    if (log_it) cout << "Rolls so far: ";
    for (int i = 0; i < rolls.size(); ++i)
        if (log_it) cout << rolls[i] << " ";
    if (log_it) cout << endl;
    while (true)
    {
        char input;
        if (log_it) cout << "Wanna roll again? (y/n)" << endl;
        cin >> input;

        if (input == 'y' || input == 'Y')
            return true;
        else if (input == 'n' || input == 'N')
            return false;
        if (log_it) cout << "Invalid input!" << endl;
    }
}

bool KiMynijo::play(vector<int> rolls, int points_other_player)
{
    int mind_value = 0;
    if (sum_vector(rolls) + points >= 100) return false;

    if (points_other_player + panic_value >= 100) return true;
    if (points + greed_for_win >= 100) return true;

    if (points + same_level_range < points_other_player)
        mind_value = losing_min_value;
    else if (points + same_level_range > points_other_player)
        mind_value = winning_min_value;
    else
        mind_value = draw_min_value;

    if (sum_vector(rolls) + points >= 100) return false;
    if (sum_vector(rolls) < mind_value)
        return true;
    else
        return false;
}

bool KiMichael::play(vector<int> rolls, int points_other_player)
{
    int x = 18;
    if (points_other_player - points >= 20) x = 19;
    if (rolls.size() > 6) return false;
    if (sum_vector(rolls) + points >= 100) return false;
    if (sum_vector(rolls) < 18)
        return true;
    else
        return false;
}
