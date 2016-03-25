# Contributing

Contributions to Sagenwelt-MUDLib are welcome as long as some guidelines are
followed:

+ Each contribution is free of any third party patents...
+ Each contribution is solemny owned by the contributer...
+ Each contribution uses it's own feature-branch...
+ Pull requests are always against the devel branch...
+ Commit messages follow the rules given by the template...

## How to keep commit messages in style

+ The repository contains two files helping in keeping a standard for commit-messages

    1.  a template
    ([origin](http://codeinthehole.com/writing/a-useful-template-for-commit-messages/)),
    whose guidelines is to follow

    It can be installed either localy by issuing

        git config --local commit.template <full path to commit.template>

    inside the repository, or you may use it together with your other
    repositories by using

        git config --global commit.template <full path to commit.template>

    At least in the later case it helps to copy the template to some
    xirectory outside the repository (e.g. ~/.git/templates/).

    2.  a hook `prepare-commit-msg`
    ([origin](http://codeinthehole.com/writing/enhancing-your-git-commit-editor/))

    This hook helps to keep all commit messages in the same style by adding
    the last five commit messages to the template.

    It can simply installed by copying it into the .git/hooks directory and
    making it executable.

+ don't ever use the '-m' option to `git commit`

### style of commit messages

Each commit message should look like this:

```
<type>(scope): subject

body-text

footer-text
```

+ type should be one of the following tags
    * **feat**: A new feature
    * **fix**: A bug fix
    * **docs**: Documentation only changes
    * **style**: Changes that do not affect the meaning of the code
    (white-space, formatting, missing semi-colons, etc)
    * **ref**: A code change that neither fixes a bug or adds a feature
    * **test**: Adding missing tests
    * **core**: Changes to the build process or auxiliary tools and libraries
    such as documentation generation
+ scope denotes the location of change and can be omitted if for some valid
reason it is too wide spread.

Should this be the case: try to split the single large commit into multiple
smaler ones!

+ subject denotes in short pregnant words what is changed.
    For bug fixes or other issues, the issue number is mandatory part of the
    subject!

The body text contains a more in depthndescription of the change and the
footer-text contains any reference links as applicable.

## Keep sync with original repo

After forking your own version of the [master
repository](https://github.com/Shea690901/Sagenwelt-Lib) and cloning it to
your local machine, add the master as upstream

    git remote add upstream git@github.com:Shea690901/Sagenwelt-Lib.git

and keep in sync with the devel branch by regularly issuing either

    git fetch upstream/devel
    git merge upstream/devel

or

    git pull upstream/devel

within your feature-branch.

## Reporting issues

Issues may be reported at
(https://github.com/Shea690901/Sagenwelt-Lib/issues).

We only accept issues that are properly labeled.

Bugs must be isolated and reproducible problems.

Please read the following guidelines before opening any issue.

1. **Search for existing issues.**

We don't want to get a lot of duplicate issues, and you'd help us out a lot
by first checking if someone else has reported the same issue. Moreover, the
issue may have already been resolved with a fix available.

2. **Create an isolated and reproducible test case.**

Be sure the problem exists in the Base MUDLib itself and not in some derived
code....

3. **Include a live example.**

Make use of screenshots if needed.

4. **Share as much information as possible.**

Include operating system and version, gamedriver and version, customized or
vanilla build, etc. where appropriate. Also include steps to reproduce the
bug.

## Key branches

- master is for releases only. A new release will be marked by an apropriate
  tag.
- devel is the main development branch. This is the branch you want to base
  your feature-branch on.
- other branches as the keep coming and going...

## Coding standards

- Please follow the guidelines given by
  [StyleCI](https://styleci.io/repos/46893775/), the configuration used is
  part of the repository. Changes to this [file](.styleci.yml) won't be
  accepted without a very good reason.
- Since the documentation system uses
  [doxygen](https://github.com/doxygen/doxygen) all needed documentation
  resides within the code itself. Please keep it this way and document
  everything.
- For those coders using vim a syntaxfile, an improvement of the one shipped
  with vim itself, is supplied within the contrib directory.
  Should you add some sefun please update this file too, even if you don't use
  vim yourself...

## License

By contributing your code, you agree to license your contribution under the
terms of the GPLv2: Read [LICENSE](LICENSE)
