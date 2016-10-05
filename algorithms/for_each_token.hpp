// http://tristanbrindle.com/posts/a-quicker-study-on-tokenising/
template <typename Iterator, typename Lambda>
void for_each_token(Iterator&& first, Iterator&& last, char separator, Lambda&& lambda)
{
    while (first != last)
    {
        const auto pos = std::find(first, last, separator);

        lambda(first, pos);

        if (pos == last)
        {
            break;
        }

        first = std::next(pos);
    }
}
