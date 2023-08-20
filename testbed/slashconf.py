import slash

# reverse the execution order of tests to top to bottom
@slash.hooks.tests_loaded.register
def tests_loaded(tests):
    for index, test in enumerate(reversed(tests)):
        test.__slash__.set_sort_key(index)