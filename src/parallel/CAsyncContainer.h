#ifndef SGL_CASYNCCONTAINER_H
#define SGL_CASYNCCONTAINER_H

#include <>

//namespace sgl {

template<typename T, template<typename> typename C>
class CAsyncContainer
{
public:
    using TType = T;
    using TContainer = C;

    void push_back(const TType& value);
    void push_back(TType&& value);

    template<typename... Types>
    void emplace_back(Types&&... args);

private:
    std::vector<TContainer<TType>> async_container_vec_; 
};

int test_CAsyncContainer
{
    using TContainer = CAsyncContainer<CFragment, CIntrinsicVector>;

    return 0;
}

//} //namespace sgl

#endif //SGL_CASYNCCONTAINER_H
