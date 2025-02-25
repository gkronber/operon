// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: Copyright 2019-2022 Heal Research

#ifndef OPERON_EVALUATOR_HPP
#define OPERON_EVALUATOR_HPP

#include <atomic>
#include <utility>

#include "operon/collections/projection.hpp"
#include "operon/core/individual.hpp"
#include "operon/core/operator.hpp"
#include "operon/core/problem.hpp"
#include "operon/core/types.hpp"
#include "operon/interpreter/interpreter.hpp"
#include "operon/operon_export.hpp"

namespace Operon {

struct OPERON_EXPORT ErrorMetric {
    using Iterator = Operon::Span<Operon::Scalar const>::iterator;
    using ProjIterator = ProjectionIterator<Iterator>;

    ErrorMetric() = default;
    ErrorMetric(ErrorMetric const&) = default;
    ErrorMetric(ErrorMetric&&) = default;
    auto operator=(ErrorMetric const&) -> ErrorMetric& = default;
    auto operator=(ErrorMetric&&) -> ErrorMetric& = default;

    virtual auto operator()(Operon::Span<Operon::Scalar const> estimated, Operon::Span<Operon::Scalar const> target) const noexcept -> double = 0;
    virtual auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double = 0;
    virtual ~ErrorMetric() = default;
};

struct OPERON_EXPORT MSE : public ErrorMetric {
    auto operator()(Operon::Span<Operon::Scalar const> estimated, Operon::Span<Operon::Scalar const> target) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double override;
};

struct OPERON_EXPORT NMSE : public ErrorMetric {
    auto operator()(Operon::Span<Operon::Scalar const> estimated, Operon::Span<Operon::Scalar const> target) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double override;
};

struct OPERON_EXPORT RMSE : public ErrorMetric {
    auto operator()(Operon::Span<Operon::Scalar const> estimated, Operon::Span<Operon::Scalar const> target) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double override;
};

struct OPERON_EXPORT MAE : public ErrorMetric {
    auto operator()(Operon::Span<Operon::Scalar const> estimated, Operon::Span<Operon::Scalar const> target) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double override;
};

struct OPERON_EXPORT R2 : public ErrorMetric {
    auto operator()(Operon::Span<Operon::Scalar const> estimated, Operon::Span<Operon::Scalar const> target) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double override;
};

struct OPERON_EXPORT C2 : public ErrorMetric {
    auto operator()(Operon::Span<Operon::Scalar const> estimated, Operon::Span<Operon::Scalar const> target) const noexcept -> double override;
    auto operator()(Iterator beg1, Iterator end1, Iterator beg2) const noexcept -> double override;
};

auto OPERON_EXPORT FitLeastSquares(Operon::Span<float const> estimated, Operon::Span<float const> target) noexcept -> std::pair<double, double>;
auto OPERON_EXPORT FitLeastSquares(Operon::Span<double const> estimated, Operon::Span<double const> target) noexcept -> std::pair<double, double>;

struct EvaluatorBase : public OperatorBase<Operon::Vector<Operon::Scalar>, Individual&, Operon::Span<Operon::Scalar>> {
    mutable std::atomic_ulong ResidualEvaluations{0}; // NOLINT
    mutable std::atomic_ulong JacobianEvaluations{0}; // NOLINT
    mutable std::atomic_ulong CallCount{0};         // NOLINT

    static constexpr size_t DefaultLocalOptimizationIterations = 50;
    static constexpr size_t DefaultEvaluationBudget = 100'000;

    using ReturnType = OperatorBase::ReturnType;

    explicit EvaluatorBase(Problem& problem)
        : problem_(problem)
    {
    }

    virtual void Prepare(Operon::Span<Individual const> pop) const
    {
    }

    auto TotalEvaluations() const -> size_t { return ResidualEvaluations + JacobianEvaluations; }

    void SetLocalOptimizationIterations(size_t value) { iterations_ = value; }
    auto LocalOptimizationIterations() const -> size_t { return iterations_; }

    void SetBudget(size_t value) { budget_ = value; }
    auto Budget() const -> size_t { return budget_; }
    auto BudgetExhausted() const -> bool { return TotalEvaluations() >= Budget(); }

    auto Population() const -> Operon::Span<Individual const> { return population_; }
    auto GetProblem() const -> Problem const& { return problem_; }
    auto GetProblem() -> Problem& { return problem_; }
    auto SetProblem(Problem& problem) { problem_ = problem; }

    void Reset()
    {
        ResidualEvaluations = 0;
        JacobianEvaluations = 0;
        CallCount = 0;
    }

    private:
    Operon::Span<Operon::Individual const> population_;
    std::reference_wrapper<Problem> problem_;
    size_t iterations_ = DefaultLocalOptimizationIterations;
    size_t budget_ = DefaultEvaluationBudget;
};

class UserDefinedEvaluator : public EvaluatorBase {
public:
    UserDefinedEvaluator(Problem& problem, std::function<typename EvaluatorBase::ReturnType(Operon::RandomGenerator&, Operon::Individual&)> func)
        : EvaluatorBase(problem)
        , fref_(std::move(func))
    {
    }

    // the func signature taking a pointer to the rng is a workaround for pybind11, since the random generator is non-copyable we have to pass a pointer
    UserDefinedEvaluator(Problem& problem, std::function<typename EvaluatorBase::ReturnType(Operon::RandomGenerator*, Operon::Individual&)> func)
        : EvaluatorBase(problem)
        , fptr_(std::move(func))
    {
    }

    auto
    operator()(Operon::RandomGenerator& rng, Individual& ind, Operon::Span<Operon::Scalar> /*args*/) const -> typename EvaluatorBase::ReturnType override
    {
        ++this->CallCount;
        return fptr_ ? fptr_(&rng, ind) : fref_(rng, ind);
    }

private:
    std::function<typename EvaluatorBase::ReturnType(Operon::RandomGenerator&, Operon::Individual&)> fref_;
    std::function<typename EvaluatorBase::ReturnType(Operon::RandomGenerator*, Operon::Individual&)> fptr_; // workaround for pybind11
};

class OPERON_EXPORT Evaluator : public EvaluatorBase {

public:
    Evaluator(Problem& problem, Interpreter& interp, ErrorMetric const& error = MSE{}, bool linearScaling = true)
        : EvaluatorBase(problem)
        , interpreter_(interp)
        , error_(error)
        , scaling_(linearScaling)
    {
    }

    auto GetInterpreter() -> Interpreter& { return interpreter_; }
    auto GetInterpreter() const -> Interpreter const& { return interpreter_; }

    auto
    operator()(Operon::RandomGenerator& /*random*/, Individual& ind, Operon::Span<Operon::Scalar> buf) const -> typename EvaluatorBase::ReturnType override;

private:
    std::reference_wrapper<Interpreter> interpreter_;
    std::reference_wrapper<ErrorMetric const> error_;
    bool scaling_{false};
};

class MultiEvaluator : public EvaluatorBase {
public:
    explicit MultiEvaluator(Problem& problem)
        : EvaluatorBase(problem)
    {
    }

    void Add(EvaluatorBase const& evaluator)
    {
        evaluators_.push_back(std::ref(evaluator));
    }

    auto Prepare(Operon::Span<Operon::Individual const> pop) const -> void override
    {
        for (auto const& e : evaluators_) {
            e.get().Prepare(pop);
        }
    }

    auto
    operator()(Operon::RandomGenerator& rng, Individual& ind, Operon::Span<Operon::Scalar> buf) const -> typename EvaluatorBase::ReturnType override
    {
        Operon::Vector<Operon::Scalar> fit;
        auto resEval{0UL};
        auto jacEval{0UL};
        auto eval{0UL};
        for (auto const& ev : evaluators_) {
            auto fitI = ev(rng, ind, buf);
            std::copy(fitI.begin(), fitI.end(), std::back_inserter(fit));

            resEval += ev.get().ResidualEvaluations;
            jacEval += ev.get().JacobianEvaluations;
            eval += ev.get().CallCount;
        }
        ResidualEvaluations = resEval;
        JacobianEvaluations = jacEval;
        CallCount = eval;
        return fit;
    }

private:
    std::vector<std::reference_wrapper<EvaluatorBase const>> evaluators_;
};

// a couple of useful user-defined evaluators (mostly to avoid calling lambdas from python)
// TODO: think about a better design
class LengthEvaluator : public UserDefinedEvaluator {
public:
    explicit LengthEvaluator(Operon::Problem& problem, size_t maxlength = 1)
        : UserDefinedEvaluator(problem, [maxlength](Operon::RandomGenerator& /*unused*/, Operon::Individual& ind) {
            return EvaluatorBase::ReturnType { static_cast<Operon::Scalar>(ind.Genotype.Length()) / static_cast<Operon::Scalar>(maxlength) };
        })
    {
    }
};

class ShapeEvaluator : public UserDefinedEvaluator {
public:
    explicit ShapeEvaluator(Operon::Problem& problem)
        : UserDefinedEvaluator(problem, [](Operon::RandomGenerator& /*unused*/, Operon::Individual& ind) {
            return EvaluatorBase::ReturnType { static_cast<Operon::Scalar>(ind.Genotype.VisitationLength()) };
        })
    {
    }
};

class OPERON_EXPORT DiversityEvaluator : public EvaluatorBase {
public:
    explicit DiversityEvaluator(Operon::Problem& problem, Operon::HashMode hashmode = Operon::HashMode::Strict)
        : EvaluatorBase(problem), hashmode_(hashmode)
    {
    }

    auto
    operator()(Operon::RandomGenerator& /*random*/, Individual& ind, Operon::Span<Operon::Scalar> buf) const -> typename EvaluatorBase::ReturnType override;

    auto Prepare(Operon::Span<Operon::Individual const> pop) const -> void override;

private:
    mutable robin_hood::unordered_flat_map<size_t, size_t> divmap_;
    mutable double total_{0}; // total count
    Operon::HashMode hashmode_;
};

} // namespace Operon
#endif
