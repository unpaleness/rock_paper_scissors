#include <iostream>
#include <ctime>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>

static inline char int_to_char(const int i)
{
    switch (i)
    {
    case 0:
        return '*'; // Rock
    case 1:
        return '|'; // Paper
    case 2:
        return 'X'; // Scissors
    default:
        return '.';
    }
}

class Field
{
public:
    Field(const size_t new_x, const size_t new_y)
    {
        x = new_x;
        y = new_y;
        storage0_ptr = std::make_shared<std::vector<int>>(x * y);
        storage1_ptr = std::make_shared<std::vector<int>>(x * y);
    }

    void init()
    {
        if (auto storage_ptr = get_storage())
        {
            for (size_t i = 0; i < storage_ptr->size(); ++i)
            {
                storage_ptr->at(i) = rand() % 3;
            }
        }
        else
        {
            std::cerr << __FUNCTION__ << ": storage is invalid";
        }
    }

    void print() const
    {
        std::cout << '\n';

        std::cout << iteration << '\n';

        if (auto storage_ptr = get_storage())
        {
            for (size_t j = 0; j < y; ++j)
            {
                for (size_t i = 0; i < x; ++i)
                {
                    std::cout << ' ' << int_to_char(storage_ptr->at(j * x + i));
                }
                std::cout << '\n';
            }
        }
        else
        {
            std::cerr << __FUNCTION__ << ": storage is invalid";
        }

        std::cout.flush();
    }

    void iterate()
    {
        ++iteration;
        const auto seed = rand() % 4;
        if (seed % 2)
        {
            play_vertically(((seed + 1) / 2) % 2);
        }
        else
        {
            play_horizontally(((seed + 1) / 2) % 2);
        }
    }

private:
    std::shared_ptr<std::vector<int>> get_storage() const { return iteration % 2 ? storage1_ptr : storage0_ptr; }
    std::shared_ptr<std::vector<int>> get_old_storage() const { return iteration % 2 ? storage0_ptr : storage1_ptr; }
    void play_vertically(const bool do_shift)
    {
        auto storage_ptr = get_storage();
        auto old_storage_ptr = get_old_storage();
        if (do_shift)
        {
            for (size_t i = 0; i < x; ++i)
            {
                sparring(i, (y - 1) * x + i);
            }
        }
        for (size_t j = do_shift ? 1 : 0; j < y - 1; j += 2)
        {
            for (size_t i = 0; i < x; ++i)
            {
                sparring(j * x + i, (j + 1) * x + i);
            }
        }
    }
    void play_horizontally(const bool do_shift)
    {
        auto storage_ptr = get_storage();
        auto old_storage_ptr = get_old_storage();
        if (do_shift)
        {
            for (size_t j = 0; j < y; ++j)
            {
                sparring(j * x, (j + 1) * x - 1);
            }
        }
        for (size_t i = do_shift ? 1 : 0; i < x - 1; i += 2)
        {
            for (size_t j = 0; j < y; ++j)
            {
                sparring(j * x + i, j * x + i + 1);
            }
        }
    }
    void sparring(const size_t player1_index, const size_t player2_index)
    {
        auto storage_ptr = get_storage();
        auto old_storage_ptr = get_old_storage();
        const auto player1 = old_storage_ptr->at(player1_index);
        const auto player2 = old_storage_ptr->at(player2_index);
        const auto diff = player2 - player1;
        const auto winner = (diff == -2 || diff == 1) ? player2 : player1;
        storage_ptr->at(player1_index) = winner;
        storage_ptr->at(player2_index) = winner;
    }

private:
    size_t x{0};
    size_t y{0};
    size_t iteration{0};
    std::shared_ptr<std::vector<int>> storage0_ptr;
    std::shared_ptr<std::vector<int>> storage1_ptr;
};

int main(int argc, char *argv[])
{
    srand(time(0));

    if (argc <= 4)
    {
        std::cerr << "Usage: <x> <y> <delay (ns)> <iterations limit>\n";
        return 1;
    }

    if (atoi(argv[1]) % 2 || atoi(argv[2]) % 2)
    {
        std::cerr << "x and y should be even\n";
        return 1;
    }

    int delay = atoi(argv[3]);
    int limit = atoi(argv[4]);
    Field field(atoi(argv[1]), atoi(argv[2]));
    field.init();
    field.print();
    for (int i = 0; i < limit; ++i)
    {
        std::this_thread::sleep_for(std::chrono::nanoseconds(delay));
        field.iterate();
        field.print();
    };
    return 0;
}
