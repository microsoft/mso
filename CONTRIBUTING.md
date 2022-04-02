# Contributing

This project welcomes contributions and suggestions. Most contributions require you to
agree to a Contributor License Agreement (CLA) declaring that you have the right to,
and actually do, grant us the rights to use your contribution. For details, visit
https://cla.microsoft.com.

When you submit a pull request, a CLA-bot will automatically determine whether you need
to provide a CLA and decorate the PR appropriately (e.g., label, comment). Simply follow the
instructions provided by the bot. You will only need to do this once across all repositories using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/)
or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.


## Additional steps for Pull Requests

0. It is recommended that you rename your remote to `upstream`.
For documentation regarding how to rename your remotes, see
[Git Basics: Working with Remotes](
https://git-scm.com/book/en/v2/Git-Basics-Working-with-Remotes)

1. Run `yarn change`

> Tip: If the output of the command is `No change files are needed`
> once you build with your changes, it is possible beachball 
> did not pickup your diff correctly. To fix this, run
> `yarn change --branch "upstream/main"` where `upstream/main` is
> your upstream remote, as per previous step.

2. Ensure the generated `change/*.json` file is present in your PR.