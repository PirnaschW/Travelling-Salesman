#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

int main()
{
  std::string path{};
  std::cout << "Enter complete input file path: ";
  std::getline(std::cin, path);
  std::ifstream inpf(path, std::ios::in);
  std::istream& input = static_cast<std::istream&>(inpf);

  std::vector<std::string> v{};
  std::string line{};
  while (std::getline(input, line)) v.emplace_back(line);

  std::vector<std::string> airport{};
  auto addAirport = [&airport](const std::string& s) -> int { for (int i = 0; i < static_cast<int>(airport.size()); ++i) { if (airport[i] == s) return i; } airport.push_back(s); return static_cast<int>(airport.size()) - 1; };

  class Flight
  {
  public:
    int from;
    int to;
    int price;
  };

  std::vector<Flight> flight{};
  for (const auto& line : v)
  {
    char from[4];
    char to[4];

    Flight f;
    int z = sscanf_s(line.c_str(), "%3c-%3c=%d", from, 3, to, 3, &f.price);
    assert(z == 3);
    from[3] = to[3] = '\0';
    f.from = addAirport(from);
    f.to = addAirport(to);
    flight.push_back(f);
  }

  std::vector<Flight> sequence{};
  int startAndEnd = addAirport("MCO");

  class Plan
  {
  public:
    std::vector<Flight> sequence{};
    int price{ 0 };
  };
  std::vector<Plan> plan{};

  auto tryToAddLocation = [&airport, &flight, &startAndEnd, &plan](const std::vector<Flight>& sequence, auto& tryToAddLocation) -> void
    {
      int n = static_cast<int>(sequence.size());

      if (n == airport.size())
      {
        Plan p{ sequence,0 };
        for (int j = 0; j < n; ++j)
        {
          p.price += sequence[j].price;
        }
        plan.push_back(p);
        return;
      }

      for (int i = 0; i < static_cast<int>(flight.size()); ++i)
      {
        if (n == 0 && flight[i].from != startAndEnd) continue;                      // must start from MCO
        if (n > 0 && flight[i].from != sequence[n - 1].to) continue;                // must start from previous flights' destination
        if (n == static_cast<int>(airport.size()) - 1 && flight[i].to != startAndEnd) continue;         // last leg must go back to MCO
        if (n < static_cast<int>(airport.size()) - 1 && flight[i].to == startAndEnd) continue;            // earlier legs must not go back to MCO

        bool reachedbefore = false;
        for (int j = 0; j < n; ++j)
        {
          if (flight[i].to == sequence[j].to)
          {
            reachedbefore = true;
            break;
          }
        }
        if (reachedbefore) continue;

        auto sequence1 = sequence;
        sequence1.push_back(flight[i]);
        tryToAddLocation(sequence1, tryToAddLocation);
      }
    };

  tryToAddLocation(sequence, tryToAddLocation);

  std::sort(plan.begin(), plan.end(), [](const Plan& p1, const Plan& p2) -> bool {return p1.price < p2.price; });
  std::cout << '\n' << plan[0].price << " - " << plan[plan.size() - 1].price << '\n';
  std::cout << "Best: ";
  std::cout << airport[plan[0].sequence[0].from] << " -> ";
  for (int j = 1; j < static_cast<int>(plan[0].sequence.size()); ++j)
  {
    std::cout << airport[plan[0].sequence[j].from] << "(" << plan[0].sequence[j - 1].price << ") -> ";
  }
  std::cout << airport[plan[0].sequence[plan[0].sequence.size() - 1].to] << "(" << plan[0].sequence[plan[0].sequence.size() - 1].price << ")" << '\n';

  std::getchar();
  return 0;
}
