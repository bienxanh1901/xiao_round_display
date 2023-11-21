Contributing

Contributions to this project in the form of pull requests, bug reports, and feature requests are welcome!

Please read the following sections if you plan to submit a PR.
Pre-commit hooks

This project uses pre-commit hooks to perform code formatting and other checks when you run git commit.

To install pre-commit hooks, run pip install pre-commit && pre-commit install.

You also need to install doxygen — it is used by one of the pre-commit hooks.

If a pre-commit hook has modified any of the files when you run git commit, add these changes using git add and run git commit again.
Release process

The version of this is tracked using Git tags. CI workflow in .github/workflows/upload_component.yml runs for every Git tag and uploads the new version to the component registry.

To make a release:

    Decide on the new version number.
    Update the changelog:
        Replace Unreleased with the new version and the date.
        Check that all the changes are mentioned.
        Add an empty Unreleased section at the top.
        Commit the updated changelog file.
    Tag the latest commit and push the tag to Github.
    Check that the CI workflow has run successfully.
