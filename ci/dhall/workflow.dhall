let GitHubActions = (./imports.dhall).GitHubActions

let shellcheck = ./jobs/shellcheck.dhall

let shfmt = ./jobs/shfmt.dhall

let checkPipeline = ./jobs/check-rendered-pipeline-up-to-date.dhall

let dhallFormat = ./jobs/dhall-format.dhall

let cFormat = ./jobs/c-format.dhall

let cBuild = ./jobs/c-build.dhall

let dhallLint = ./jobs/dhall-lint.dhall

let dhallCheck = ./jobs/dhall-check.dhall

let prettier = ./jobs/prettier.dhall

in  GitHubActions.Workflow::{
    , name = "CI"
    , on = GitHubActions.On::{ push = Some GitHubActions.Push::{=} }
    , jobs = toMap
        { shellcheck
        , shfmt
        , dhallCheck
        , cFormat
        , cBuild
        , dhallFormat
        , dhallLint
        , checkPipeline
        , prettier
        }
    }
