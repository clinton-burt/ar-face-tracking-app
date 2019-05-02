#ifndef PARTICLE_DATA_TYPES_H_INCLUDED
#define PARTICLE_DATA_TYPES_H_INCLUDED

#include <map>
#include <random>
#include <chrono>

#define _USE_MATH_DEFINES
#include <math.h>

#include <Eigen/Geometry>

#include <boost/property_tree/ptree.hpp>

#include "JSONVectorReader.h"

namespace chrono = std::chrono;
namespace pt = boost::property_tree;
using namespace Eigen;

static class SalmonRandom
{
protected:
	static std::mt19937 generator;

public:
	static int RandomInt(int a, int b);
	static double RandomReal(double a, double b);
};


// < TYPES WRAPPERS >

using uint = unsigned int;

struct UintWrapper
{
	uint v;
	UintWrapper() {}
	UintWrapper(const uint& other)
	{
		v = other;
	}
	UintWrapper& operator=(const uint& other)
	{
		v = other;
		return *this;
	}
	inline uint get() const
	{
		return v;
	}
	static UintWrapper Zero()
	{
		return{ 0 };
	}
};

struct FloatWrapper
{
	float v;
	FloatWrapper() {}
	FloatWrapper(const float& other)
	{
		v = other;
	}
	FloatWrapper& operator=(const float& other)
	{
		v = other;
		return *this;
	}
	inline float get() const
	{
		return v;
	}
	static FloatWrapper Zero()
	{
		return{ 0 };
	}
};

struct Vector2fWrapper
{
	Vector2f v;
	Vector2fWrapper() {}
	Vector2fWrapper(const Vector2f& other)
	{
		v = other;
	}
	Vector2fWrapper& operator=(const Vector2f& other)
	{
		v = other;
		return *this;
	}
	inline Vector2f get() const
	{
		return v;
	}
	static Vector2fWrapper Zero()
	{
		return{ { 0, 0 } };
	}
};

struct Vector3fWrapper
{
	Vector3f v;
	Vector3fWrapper() {}
	Vector3fWrapper(const Vector3f& other)
	{
		v = other;
	}
	Vector3fWrapper& operator=(const Vector3f& other)
	{
		v = other;
		return *this;
	}
	inline Vector3f get() const
	{
		return v;
	}
	static Vector3fWrapper Zero()
	{
		return{ { 0, 0, 0 } };
	}
};

struct Vector4fWrapper
{
	Vector4f v;
	Vector4fWrapper() {}
	Vector4fWrapper(const Vector4f& other)
	{
		v = other;
	}
	Vector4fWrapper& operator=(const Vector4f& other)
	{
		v = other;
		return *this;
	}
	inline Vector4f get() const
	{
		return v;
	}
	static Vector4fWrapper Zero()
	{
		return{ { 0, 0, 0, 0 } };
	}
};

// < / TYPES WRAPPERS >

// < TYPES RANDOMS >

struct RandomUint
{
	uint min, max;
	uint random() const
	{
		return uint(SalmonRandom::RandomInt(min, max));
	}
	void load(const pt::ptree& record)
	{
		auto value = record.get_optional<uint>("value");
			
		if (value)
		{
			min = value.get();
			max = min;
		}
		else
		{
			min = record.get<uint>("min");
			max = record.get<uint>("max");
		}
	}
};

struct RandomFloat
{
	FloatWrapper min, max;
	FloatWrapper random() const
	{
		return FloatWrapper(SalmonRandom::RandomReal(min.get(), max.get()));
	}
	void load(const pt::ptree& record)
	{
		auto value = record.get_optional<float>("value");

		if (value)
		{
			min = value.get();
			max = min;
		}
		else
		{
			min = record.get<float>("min");
			max = record.get<float>("max");
		}
	}
};

struct RandomVector2f
{
	Vector2fWrapper min, max;
	Vector2fWrapper random() const
	{
		float a = SalmonRandom::RandomReal(min.v[0], max.v[0]);
		float b = SalmonRandom::RandomReal(min.v[1], max.v[1]);
		return{ { a, b } };
	}
	void load(const pt::ptree& record)
	{
		auto value = record.get_child_optional("value");

		if (value)
		{
			min = JSONVectorReader::readVector2f(const_cast<pt::ptree&>(value.get()));
			max = min;
		}
		else
		{
			min = JSONVectorReader::readVector2f(const_cast<pt::ptree&>(record.get_child("min")));
			max = JSONVectorReader::readVector2f(const_cast<pt::ptree&>(record.get_child("max")));
		}
	}
};

struct RandomVector3f
{
	Vector3fWrapper min, max;
	Vector3fWrapper random() const
	{
		float a = SalmonRandom::RandomReal(min.v[0], max.v[0]);
		float b = SalmonRandom::RandomReal(min.v[1], max.v[1]);
		float c = SalmonRandom::RandomReal(min.v[2], max.v[2]);
		return{ { a, b, c } };
	}
	void load(const pt::ptree& record)
	{
		auto value = record.get_child_optional("value");

		if (value)
		{
			min = JSONVectorReader::readVector3f(const_cast<pt::ptree&>(value.get()));
			max = min;
		}
		else
		{
			min = JSONVectorReader::readVector3f(const_cast<pt::ptree&>(record.get_child("min")));
			max = JSONVectorReader::readVector3f(const_cast<pt::ptree&>(record.get_child("max")));
		}
	}
};

struct RandomVector4f
{
	Vector4fWrapper min, max;
	Vector4fWrapper random() const
	{
		float a = SalmonRandom::RandomReal(min.v[0], max.v[0]);
		float b = SalmonRandom::RandomReal(min.v[1], max.v[1]);
		float c = SalmonRandom::RandomReal(min.v[2], max.v[2]);
		float d = SalmonRandom::RandomReal(min.v[3], max.v[3]);
		return{ { a, b, c, d } };
	}
	void load(const pt::ptree& record)
	{
		auto value = record.get_child_optional("value");

		if (value)
		{
			min = JSONVectorReader::readVector4f(const_cast<pt::ptree&>(value.get()));
			max = min;
		}
		else
		{
			min = JSONVectorReader::readVector4f(const_cast<pt::ptree&>(record.get_child("min")));
			max = JSONVectorReader::readVector4f(const_cast<pt::ptree&>(record.get_child("max")));
		}
	}
};

// < / TYPES RANDOMS >




// < BASE >

template <typename T>
struct ValueAtMoment
{
	float t;
	T v;

	ValueAtMoment(float t, T v) : t(t), v(v) { }
};

template <typename T, typename R>
struct RandomValueAtMoment
{
	float t;
	R v;
	ValueAtMoment<T> random() const
	{
		return{ t, v.random() };
	}
	void load(const pt::ptree& record)
	{
		v.load(record);
		t = record.get<float>("t", 0);
	}
};

enum class TimelineType
{
	CONTINUED,
	LIMITED,
	LOOPED
};

enum class InterpolatorType
{
	DISCRETE,
	LINEAR_SPLINE,
	CUBIC_SPLINE
};

template <typename T>
struct Interpolator
{
	TimelineType timelineType;
	float timeOffset;

	std::vector<ValueAtMoment<T>> knots;

	Interpolator(const std::vector<ValueAtMoment<T>>& knots, TimelineType timelineType, float timeOffset)
	{
		this->knots = knots;
		this->timelineType = timelineType;
		this->timeOffset = timeOffset;
	}

	virtual InterpolatorType getType() = 0;

	virtual T at(float t) = 0;

	template <class F>
	static std::shared_ptr<Interpolator<T>> init(const std::vector<ValueAtMoment<T>>& knots, TimelineType timelineType, float timeOffset)
	{
		return std::make_shared<F>(knots, timelineType, timeOffset);
	}
};

template <typename T>
struct DiscreteFunction : public Interpolator<T>
{
	DiscreteFunction(const std::vector<ValueAtMoment<T>>& knots, TimelineType timelineType, float timeOffset)
		: Interpolator<T>(knots, timelineType, timeOffset) { }

	InterpolatorType getType() override
	{
		return InterpolatorType::DISCRETE;
	}

	T at(float t) override
	{
		if (knots.empty())
		{
			return T::Zero();
		}

		if (knots.size() == 1)
		{
			return knots.front().v;
		}

		t += timeOffset; bool skipSearch = false;
		int i;

		if (t > knots.back().t)
		{
			switch (timelineType)
			{
			case TimelineType::LOOPED:
				if (knots.back().t > 0.001f)
				{
					int n = (int)(t / knots.back().t);
					t -= n * knots.back().t;
				}
				else
				{
					i = knots.size() - 1;
					skipSearch = true;
				}
				break;

			case TimelineType::LIMITED:
				i = knots.size() - 1;
				skipSearch = true;
				break;

			default:
				break;
			}
		}

		if (!skipSearch)
		{
			i = 0;
			for (auto iter = ++knots.begin(); iter != knots.end(); ++iter, ++i)
			{
				if (t < iter->t)
				{
					break;
				}
			}
		}

		return knots[i].v;
	}
};

template <typename T>
struct LinearSpline : public Interpolator<T>
{
	LinearSpline(const std::vector<ValueAtMoment<T>>& knots, TimelineType timelineType, float timeOffset)
		: Interpolator<T>(knots, timelineType, timeOffset) { }

	InterpolatorType getType() override
	{
		return InterpolatorType::LINEAR_SPLINE;
	}

	T at(float t) override
	{
		if (knots.empty())
		{
			return T::Zero();
		}

		if (knots.size() == 1)
		{
			return knots.front().v;
		}

		t += timeOffset;

		bool skipSearch = false;
		int i;

		if (t > knots.back().t)
		{
			switch (timelineType)
			{
			case TimelineType::LOOPED:
				if (knots.back().t > 0.001f)
				{
					int n = (int)(t / knots.back().t);
					t -= n * knots.back().t;
				}
				else
				{
					i = knots.size() - 1;
					skipSearch = true;
				}
				break;

			case TimelineType::LIMITED:
				i = knots.size() - 1;
				skipSearch = true;
				break;

			default:
				break;
			}
		}

		if (!skipSearch)
		{
			auto iter = ++(knots.begin());
			i = 1;
			for (; iter != knots.end(); ++iter)
			{
				if (t < iter->t)
				{
					break;
				}
			}
		}

		return knots[i - 1].v.get() + (knots[i].v.get() - knots[i - 1].v.get()) * ((t - knots[i - 1].t) / (knots[i].t - knots[i - 1].t));
	}
};

template <typename T>
struct CubicSpline : public Interpolator<T>
{
	std::vector<T> m;
	std::vector<float> h;

	InterpolatorType getType() override
	{
		return InterpolatorType::CUBIC_SPLINE;
	}

	CubicSpline(const std::vector<ValueAtMoment<T>>& knots, TimelineType timelineType, float timeOffset) :
		Interpolator<T>(knots, timelineType, timeOffset)
	{
		int n_1 = knots.size();
		int n = knots.size() - 1;

		std::vector<float> a;
		std::vector<float> b;
		std::vector<float> c;
		std::vector<T> d;
		std::vector<float> alpha;
		std::vector<T> beta;

		a.resize(n_1);
		b.resize(n_1);
		c.resize(n_1);
		d.resize(n_1);
		alpha.resize(n_1);
		beta.resize(n_1);

		m.resize(n_1);
		h.resize(n);

		for (int i = 0; i < n; ++i)
		{
			h[i] = knots[i + 1].t - knots[i].t;
		}

		a[0] = 1;
		b[0] = 0;
		c[0] = std::numeric_limits<float>::signaling_NaN();
		d[0] = T::Zero();

		for (int i = 1; i < n; ++i)
		{
			a[i] = (h[i - 1] + h[i]) / 3;
			b[i] = h[i] / 6;
			c[i] = h[i - 1] / 6;
			d[i] = (knots[i + 1].v.get() - knots[i].v.get()) / h[i] - (knots[i].v.get() - knots[i - 1].v.get()) / h[i - 1];
		}

		a[n] = 1;
		b[n] = std::numeric_limits<float>::signaling_NaN();
		c[n] = 0;
		d[n] = T::Zero();

		alpha[0] = -(b[0] / a[0]);
		beta[0] = d[0].get() / a[0];

		for (int i = 1; i < n_1; ++i)
		{
			alpha[i] = -b[i] / (a[i] + c[i] * alpha[i - 1]);
			beta[i] = (d[i].get() - c[i] * beta[i - 1].get()) / (a[i] + c[i] * alpha[i - 1]);
		}

		m[n] = beta[n];
		for (int i = n - 1; i > -1; --i)
		{
			m[i] = alpha[i] * m[i + 1].get() + beta[i].get();
		}
	}

	T at(float t) override
	{
		if (knots.empty())
		{
			return T::Zero();
		}

		if (knots.size() == 1)
		{
			return knots.front().v;
		}

		t += timeOffset;

		int i;
		bool skipSearch = false;

		if (t > knots.back().t)
		{
			switch (timelineType)
			{
			case TimelineType::LOOPED:
				if (knots.back().t > 0.001f)
				{
					int n = (int)(t / knots.back().t);
					t -= n * knots.back().t;
				}
				else
				{
					i = knots.size() - 1;
					skipSearch = true;
				}
				break;

			case TimelineType::LIMITED:
				i = knots.size() - 1;
				skipSearch = true;
				break;

			default:
				break;
			}
		}

		if (!skipSearch)
		{
			i = 1;
			auto iter = ++(knots.begin());
			for (; iter != knots.end(); ++iter, ++i)
			{
				if (t < iter->t)
				{
					break;
				}
			}
		}

		float diffRight = knots[i].t - t;
		float diffLeft = t - knots[i - 1].t;
		float hi6 = 6 * h[i - 1];
		float hiQuad = h[i - 1] * h[i - 1];

		return (diffRight * diffRight * diffRight - hiQuad * diffRight) / hi6 * m[i - 1].get() +
			(diffLeft * diffLeft * diffLeft - hiQuad * diffLeft) / hi6 * m[i].get() +
			diffRight / h[i - 1] * knots[i - 1].v.get() + diffLeft / h[i - 1] * knots[i].v.get();

	}
};

template <typename T, typename R>
struct RandomValueAtTimeline
{
	template <typename H = T>
	using InterpolatorIniter = std::function<std::shared_ptr<Interpolator<H>>
		(const std::vector<ValueAtMoment<H>>& knots, TimelineType timelineType, float timeOffset)>;

	static const std::map<std::string, InterpolatorIniter<>> InterpolatorIniterMap;
	static const std::map<std::string, TimelineType> TimelineTypeMap;

	InterpolatorIniter<> interpolatorIniter;
	TimelineType timelineType;

	RandomFloat timeOffset;

	bool matchDeathTime; // is used outside the struct to set "endTime" parameter for random()

	std::vector<RandomValueAtMoment<T, R>> timeline;

	std::shared_ptr<Interpolator<T>> random(float endTime = -1) const
	{
		float factor = endTime == -1 ? 1 : endTime / timeline.back().t;
		std::vector<ValueAtMoment<T>> result;
		for (auto &elem : timeline)
		{
			auto resultElem = elem.random();
			resultElem.t = resultElem.t * factor;
			result.push_back(resultElem);
		}

		return interpolatorIniter(result, timelineType, timeOffset.random().get());
	}

	void load(const pt::ptree& record)
	{
		matchDeathTime = record.get<bool>("matchDeathTime", false);

		auto optional = record.get_child_optional("value");

		auto timeOffsetTree = record.get_child_optional("timeOffset");
		if (timeOffsetTree)
		{
			timeOffset.load(timeOffsetTree.get());
		}
		else
		{
			timeOffset.min = 0;
			timeOffset.max = 0;
		}

		timeline.clear();

		RandomValueAtMoment<T, R> value;

		if (!optional)
		{
			auto interpolation = record.get<std::string>("interpolation");
			interpolatorIniter = InterpolatorIniterMap.at(interpolation);

			timelineType = TimelineTypeMap.at(record.get<std::string>("timelineType", "CONTINUED"));

			for (auto &elem : record.get_child("timeline"))
			{
				value.load(elem.second);
				timeline.push_back(value);
			}
		}
		else
		{
			value.load(record);
			timeline.push_back(value);
			timelineType = TimelineType::CONTINUED;
			interpolatorIniter = InterpolatorIniterMap.at("DISCRETE");
		}
	}
};

template <typename T, typename R>
const std::map<std::string, RandomValueAtTimeline<T, R>::InterpolatorIniter<>> RandomValueAtTimeline<T, R>::InterpolatorIniterMap =
{
	{ "DISCRETE", Interpolator<T>::init<DiscreteFunction<T>> },
	{ "LINEAR_SPLINE", Interpolator<T>::init<LinearSpline<T>> },
	{ "CUBIC_SPLINE", Interpolator<T>::init<CubicSpline<T>> }
};

template <typename T, typename R>
const std::map<std::string, TimelineType> RandomValueAtTimeline<T, R>::TimelineTypeMap =
{
	{ "CONTINUED", TimelineType::CONTINUED },
	{ "LIMITED", TimelineType::LIMITED },
	{ "LOOPED", TimelineType::LOOPED }
};

// < / BASE >

#endif