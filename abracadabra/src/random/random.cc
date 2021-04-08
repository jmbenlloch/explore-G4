#include "random/random.hh"

#include <Randomize.hh>

#include <cmath>
#include <stack>
#include <numeric>

// Random result generation utilities
inline G4double uniform    ()                        { return G4Random().flat(); }
inline bool     biased_coin(G4double chance_of_true) { return uniform() < chance_of_true; }
inline unsigned fair_die   (unsigned sides)          { return std::floor(uniform() * sides); }

// Stack utilities
using STACK = std::stack<unsigned>;
inline bool something_in(const STACK& stack) { return ! stack.empty(); }
unsigned pop(STACK& stack) { auto element = stack.top(); stack.pop(); return element; }

// Vose's adaptation of Walker's alias method
biased_choice::biased_choice(std::vector<G4double> weights)
  : prob (weights.size(), 0)
  , topup(weights.size(), 0)
{
  const unsigned N = weights.size();
  // Normalize histogram
  auto total_weight = std::accumulate(begin(weights), end(weights), 0e0);
  for (auto& weight : weights) {
    weight *= (N / total_weight);
    std::cout << "weight " << weight << std::endl;
  }
  // Separate into above-average and below-average probabilities
  std::stack<unsigned> under;
  std::stack<unsigned>  over;
  unsigned n = 0;
  for (auto weight : weights) {
    if (weight < 1) { under.push(n++); }
    else            {  over.push(n++); }
  }
  // Top up each underfilled bin, with portion of an overfilled bin
  while (something_in(over) && something_in(under)) {
    auto underfilled = pop(under);
    auto  overfilled = pop(over);
    prob [underfilled] = weights[underfilled];
    topup[underfilled] = overfilled;
    weights[overfilled] += weights[underfilled] - 1; // Odd order: more numerically stable calculation
    if (weights[overfilled] < 1) { under.push(overfilled); } // Became underfull: move to underfulls
    else                         {  over.push(overfilled); } // Still overfull: put back with overfulls
  }
  // Only above-average remain: fill their bins to the top
  while (something_in( over)) { auto i = pop( over); prob[i] = 1; }
  while (something_in(under)) { auto i = pop(under); prob[i] = 1; } // Impossible without numerical instability
}

unsigned biased_choice::operator()() {
  auto n = fair_die(prob.size());
  return biased_coin(prob[n]) ? n : topup[n];
}
