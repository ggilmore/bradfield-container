let GitHubActions = (../imports.dhall).GitHubActions

let Setup = ../setup.dhall

in  Setup.MakeJob
      Setup.JobArgs::{
      , name = "c-build"
      , additionalSteps =
        [ GitHubActions.Step::{
          , name = Some "Check to see that container code builds"
          , run = Some "just build-c"
          }
        ]
      }
