#include <iostream>
#include <fstream>
#include <assert.h>

#define TESTING 1

// We first define our two data structures Grid and User
typedef struct User
{
    int speed;
    int data;
    int factor;
} User;

typedef struct Score
{
    float speed;
    int data;
} Score;

// defined in computation section
int flr(float speed);
void speed_col(User** users, float* speed_matrix, int col, int* ids, int m);

class Grid
{
private:
    // Private variables
    int m;
    int n;
    int alpha;
    int* ids;

    // Private helper functions
    int place_user(int user_id, int col, int row)
    {
        if (row < 0 || m <= row || col < 0 || n <= col)
            return -1;
        ids[col * m + row] = user_id;
        return 0;
    }

    int get_top(int col)
    {
        int start = col * m;
        int cur = 0;
        for (int cur = 0; cur < m; cur++)
            if (ids[start + cur] == 0)
                return cur;
        return cur;
    }

    int remove_user(int user_id, int col)
    {
        int start = col * m;
        int cur = 0;
        while (ids[start + cur] != user_id && cur < m)
            cur++;
        
        if (cur == m)
            return -1;

        int rem = m - cur - 1;
        //we then shift all values after
        for (int i = 0; i < rem; i++)
        {
            ids[start + cur + i] = ids[start + cur + i + 1];
        }
        ids[start + cur + rem] = 0;
        return start + cur;
    }

    int excluded(int user_id, int col)
    {
        int start = col * m;
        for (int cur = 0; cur < m; cur++)
            if (ids[start + cur] == user_id)
                return 0;
        return 1;
    }

    int length(int user_id, int col)
    {
        int start = col * m;
        int cur = 0;
        int within = 0;
        while (ids[start + cur] != 0 && cur < m)
        {
            if (ids[start + cur] == user_id)
                within = 1;
            cur++;
        }
        return cur * within;
    }

    int best_without(int user_id, int o_user_id)
    {
        // get column with the least users or -1 if none
        int score = n;
        int col = -1;
        int count;
        for (int i = 0; i < n; i++)
        {
            count = length(user_id, i);
            if (count < score && count > 0 && excluded(o_user_id, i))
            {
                score = count;
                col = i;
            }
        }
        return col;
    }

    int worst_without(int user_id, int o_user_id)
    {
        // get column with the most users or -1 if none without o_user_id
        int score = 0;
        int col = -1;
        int count;
        int exc;
        for (int i = 0; i < n; i++)
        {
            count = length(user_id, i);
            exc = excluded(o_user_id, i);
            if (count > score && exc > 0)
            {
                score = count;
                col = i;
            }
        }
        return col;
    }

public:
    // (De)Constructors
    Grid(int* header)
    {
        m = header[0];
        n = header[1];
        alpha = header[3];
        ids = new int[m * n];
        for (int i = 0; i < m * n; i++)
            ids[i] = 0;
    }

    ~Grid()
    {
        delete ids;
    }

    // Public function interface
    int push(int user_id, int col)
    {
        int row = get_top(col);
        return place_user(user_id, col, row);
    }

    int pop(int col)
    {
        int row = get_top(col);
        return place_user(0, col, row - 1);
    }

    int swap(int user_id_e, int user_id_l)
    {
        // swap the user_id_e with excess from its best col with user_id_l worst 
        int best_col = best_without(user_id_e, user_id_l);
        int worst_col = worst_without(user_id_l, user_id_e);
        if (best_col == -1 || worst_col == -1)
            return -1;
        
        int ret;
        ret = remove_user(user_id_e, best_col);
        if (ret == -1)
            return -1;
        ret = remove_user(user_id_l, worst_col);
        if (ret == -1)
        {
            // recover old state
            push(user_id_e, best_col);
            return -1;
        }
        ret = push(user_id_e, worst_col);
        ret = push(user_id_l, best_col);
        return ret;
    }

    void compute_speeds(User** users, int num_users, float* speed_matrix)
    {
        for (int i = 0; i < m * n; i++)
            speed_matrix[i] = 0.0;

        for (int col = 0; col < n; col++)
            speed_col(users, speed_matrix, col, ids + (col * m), m);
    }


    void mk_scores(Score** scores, int num_users, float* speed_matrix,
                    int* speed_map)
    {
        int tally[num_users];
        for (int i = 0; i < num_users; i++)
        {
            scores[i]->speed = 0.0;
            scores[i]->data = 0;
            tally[i] = 0;
        }

        float speed;
        int id;
        for (int col = 0; col < n; col++)
            for (int row = 0; row < m; row++)
            {
                id = ids[col * m + row];
                if (id == 0)
                    break;
                speed = speed_matrix[col * m + row];
                scores[id - 1]->speed += speed;
                scores[id - 1]->data += speed_map[flr(speed)];
                tally[id - 1] += 1;
            }

        // average out the speed for the scores
        for (int i = 0; i < num_users; i++)
            scores[i]->speed /= std::max(1, tally[i]);
    }

    int* copy_ids()
    {
        // output the transpose (the original matrix)
        int s = m * n;
        int* new_ids = new int[s];
        for (int row = 0; row < m; row++)
            for (int col = 0; col < n; col++)
                new_ids[row * n + col] = ids[col * m + row];
        
        return new_ids;
    }

    void show()
    {
        for (int row = 0; row < m; row++)
        {
            for (int col = 0; col < n; col++)
            {
                int user_id = ids[col * m + row];
                if (user_id)
                {
                    std::cout << "U" << user_id;
                }
                else
                {
                    std::cout << "-";
                }
                if (col != n - 1)
                {
                    std::cout << ",";
                }
            }
            std::cout << std::endl;
        }
    }
};

// We then define functions for loading inputs from files

// header should be an array of length 4
void load_header(int* header, const char* fname)
{
    std::fstream fs;
    fs.open(fname, std::fstream::in);
    int cur;
    fs >> cur;
    header[0] = cur;
    fs.get();
    fs >> cur;
    header[1] = cur;
    fs >> cur;
    header[2] = cur;
    fs >> cur;
    header[3] = cur;
    fs.close();
}

// call load_header first to determine the right allocation size of
// users
void load_users(User** users, int num_users, const char* fname)
{
    std::fstream fs;
    fs.open(fname, std::fstream::in);
    int cur;

    // traverse the header
    fs >> cur;
    fs.get();
    fs >> cur;
    fs.get();
    fs >> cur;
    fs.get();
    fs >> cur;
    fs.get();

    User* user;
    for (int i = 0; i < num_users; i++)
    {
        user = users[i];
        fs >> cur;      // user id which is implicit
        fs.get();
        fs >> cur;      // user speed
        fs.get();
        user->speed = (float) cur;
        fs >> cur;      // user data
        fs.get();
        user->data = cur;
        fs >> cur;      // user factor
        fs.get();
        user->factor = cur;
    }
    fs.close();
}

void load_speed2bw(const char* fname, int* s2bw_map, int map_size)
{
    int cur;

    std::fstream fs;
    fs.open(fname, std::fstream::in);
    for (int i = 0; i < map_size; i++)
    {
        fs >> cur;      // array index which is implicit from speed
        fs.get();
        fs >> cur;      // the actual mapping
        s2bw_map[i] = cur;
    }
    fs.close();
}



// We then do the computation stuff
int flr(float speed)
{
    return (speed + 1e-6);
}

void speed_col(User** users, float* speed_matrix, int col, int* ids, int m)
{
    int start = col * m;
    int row, id;
    int total_factor = 0;
    float dep;
    for (row = 0; row < m; row++)
    {
        id = ids[row];
        if (id == 0)
            break;
        total_factor += users[id - 1]->factor;
    }
    
    for (row = 0; row < m; row++)
    {
        id = ids[row];
        if (id == 0)
            break;
        int factor = users[id - 1]->factor;
        dep = (factor * (total_factor - factor)) / 10000.0;
        speed_matrix[start + row] = std::max((float) 0.0,
                                             (float) users[id - 1]->speed * (1 - dep));
    }
}

void show_scores(User** users, Score** scores, int num_users, int alpha)
{
    int max_speed = 0;
    int max_data = 0;
    float objective = 0.0;
    float penalty = 0.0;
    int pen;
    for (int i = 0; i < num_users; i++)
    {
        max_speed += users[i]->speed;
        max_data += users[i]->data;
        objective += scores[i]->speed;
        penalty += std::max((float) 0.0,
                            (float) users[i]->data - scores[i]->data);
    }

    objective /= max_speed;
    penalty /= (float) max_data;
    for (int i = 0; i < num_users; i++)
    {
        pen = std::max((float) 0.0,
                       (float) users[i]->data - scores[i]->data);
        std::cout << pen << ",";
    }
    std::cout << penalty << std::endl;

    for (int i = 0; i < num_users; i++)
    {
        std::cout << scores[i]->speed << ",";
    }
    std::cout << objective << std::endl;
    std::cout << objective - alpha * penalty << std::endl;
}

void test();

int main(void)
{
#if TESTING
    test();
#endif
    return 0;
}


void test()
{
    std::cout << "starting test..." << std::endl;
    int header[4];
    load_header(header, "toy_example/toy_testcase.csv");
    int m = header[0];
    int n = header[1];
    int num_users = header[2];
    int alpha = header[3];
    int s = n * m;
    assert(m == 3);
    assert(n == 5);
    assert(num_users == 4);
    assert(alpha == 100);

    User* users[num_users];
    for (int i = 0; i < num_users; i++)
        users[i] = new User;

    load_users(users, num_users, "toy_example/toy_testcase.csv");

    assert(users[0]->speed  ==   20);
    assert(users[0]->data   == 4000);
    assert(users[0]->factor ==   30);

    assert(users[1]->speed ==    15);
    assert(users[1]->data ==   8000);
    assert(users[1]->factor ==   25);

    assert(users[2]->speed ==    26);
    assert(users[2]->data ==   8200);
    assert(users[2]->factor ==   60);

    assert(users[3]->speed ==    20);
    assert(users[3]->data ==  18000);
    assert(users[3]->factor ==   40);

    Grid grid = Grid(header);
    grid.push(4, 0);
    grid.push(3, 0);
    grid.push(4, 1);
    grid.push(2, 1);
    grid.push(3, 2);
    grid.push(1, 2);
    grid.push(4, 3);
    grid.push(2, 3);
    grid.push(4, 4);
    grid.push(2, 4);
    int t_ids[15] = {4,4,3,4,4,3,2,1,2,2,0,0,0,0,0};
    int* c_ids = grid.copy_ids();
    for (int i = 0; i < 15; i++)
        assert(t_ids[i] == c_ids[i]);

    int map_size = 27;
    int t_sm[map_size]
        = {0,290,575,813,1082,1351,1620,1889,2158,2427,2696
          ,2965,3234,3503,3772,4041,4310,4579,4848,5117,5386
          ,5655,5924,6093,6360,6611,6800
          };
    int* speed_map = new int[map_size];
    Score* scores [num_users];
    for (int i = 0; i < num_users; i++)
        scores[i] = new Score;

    float* speed_matrix = new float[s];
    load_speed2bw("toy_example/speed_to_map.csv", speed_map, map_size);
    for (int i = 0; i < map_size; i++)
        assert(speed_map[i] == t_sm[i]);

    grid.compute_speeds(users, num_users, speed_matrix);
    grid.mk_scores(scores, num_users, speed_matrix, speed_map);
    grid.show();
    show_scores(users, scores, num_users, alpha);

    grid.push(3, 4);
    assert(-1 == grid.swap(3,1));
    assert(0 == grid.swap(1,4));
    assert(0 == grid.swap(2,3));

    delete c_ids;
    c_ids = grid.copy_ids();
    assert(c_ids[7] == 4);
    assert(c_ids[14] == 1);
    assert(c_ids[5] == 2);
    assert(c_ids[6] == 3);

    delete [] speed_matrix;
    delete [] speed_map;
    delete c_ids;
    for (int i = 0; i < num_users; i++)
    {
        delete users[i];
        delete scores[i];
    }

    std::cout << "tests ok..." << std::endl;
}
