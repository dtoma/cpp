template <typename T>
void walk_dir(std::string const &fpath, T &&function)
{
    char const *filepath[] = {fpath.c_str(), nullptr};
    auto *tree = fts_open(
        const_cast<char *const *>(filepath),
        FTS_LOGICAL | FTS_NOSTAT,
        [](auto const **, auto const **) { return 0; });

    if (tree == nullptr)
    {
        std::cerr << "Error opening tree\n";
        throw std::runtime_error("fts_open failure");
    }

    FTSENT *s;
    while ((s = fts_read(tree)))
    {
        switch (s->fts_info)
        {
        case FTS_DNR: /* Cannot read directory */
        case FTS_ERR: /* Miscellaneous error */
        case FTS_NS:  /* stat() error Show error, then continue to next files. */
            std::cerr << "Error on " << s->fts_path << "\n";
            continue;
        case FTS_DP: /* Ignore post-order visit to directory. */
            continue;
        }

        function(s);
    }

    fts_close(tree);
}

// Example usage
std::vector<std::string> find_paths(const std::string &root, const std::string &regexStr)
{
    std::vector<std::string> paths;
    std::regex r(regexStr);

    walk_dir(root, [&paths, &r](auto *f) {
        if (std::regex_match(f->fts_path, r))
        {
            paths.push_back(f->fts_path);
        }
    });

    return paths;
}
