/* •¶šƒR[ƒh‚Í‚r‚i‚h‚r ‰üsƒR[ƒh‚Í‚b‚q‚k‚e */
/* $Id$ */

#pragma once

template<typename C>
class data_from_tuple_container
{
public:
	typedef typename C::value_type sample;
	typedef typename C::const_iterator container_iterator;

	explicit data_from_tuple_container(C&& c) : m_c(std::forward<C>(c))
	{
	}

	data_from_tuple_container(data_from_tuple_container&& ds) :
		m_c(std::forward<C>(ds.m_c))
	{
	}

	enum { arity = std::tuple_size<typename C::value_type>::value };

	class iterator
	{
	public:
		explicit iterator(container_iterator itr) : m_itr(itr) {}

		void operator++() { ++m_itr; }
		sample operator*() const { return *m_itr; }

	private:
		container_iterator m_itr;
	};

	data::size_t size() const { return m_c.size(); }
	iterator begin() const { return iterator(m_c.begin()); }

private:
	C m_c;
};

namespace boost {
	namespace unit_test {
		namespace data {
			namespace monomorphic {
				template<typename C>
				struct is_dataset<data_from_tuple_container<C>> : mpl::true_ {};
			}
		}
	}
}

extern void* enabler;

template<typename T,
	typename std::enable_if<
		std::is_base_of<std::forward_iterator_tag, typename T::iterator::iterator_category>::value &&
		std::tuple_size<typename T::value_type>::value != 0
	>::type*& = enabler>
auto make_data_from_tuple_container(T c)
{
	return data_from_tuple_container<T>(std::forward<T>(c));
}
