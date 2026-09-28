# μTRACE 0x0000000C — OBSERVE / implementation-level tails

parent_mu=0x0000000B
phase=OBSERVE
function=CONTROL_GRAFO
procedure_id=ACTIMPL-0002
timestamp_local=2026-09-27T21:31-03:00
evidence_state=OBSERVED
claim_allowed=false

## addnab/docker-run-action
Dockerfile:
FROM docker:20.10
RUN apk add bash
ENTRYPOINT /entrypoint.sh

entrypoint.sh:
- optional docker login using INPUT_USERNAME/INPUT_PASSWORD/INPUT_REGISTRY
- transforms INPUT_RUN newlines into semicolon-delimited script
- appends INPUT_DOCKER_NETWORK into options
- executes docker run with host /var/run/docker.sock mounted
- expands INPUT_OPTIONS, INPUT_SHELL, INPUT_IMAGE and generated command text

Security/reproducibility classification:
DOCKER_SOCKET_MOUNT=OBSERVED
INPUT_TO_SHELL_COMMAND=OBSERVED
REGISTRY_CREDENTIAL_ROUTE=OBSERVED_CONDITIONAL
BASE_IMAGE_DIGEST_PIN=NOT_OBSERVED (docker:20.10 tag)
DELIBERATE_OBFUSCATION=NOT_PROVEN

## actions/checkout
Human TypeScript source confirms:
- getInputs -> gitSourceProvider.getSource
- Git fetch/checkout and optional REST fallback
- auth configuration before fetch
- token transformed into Basic auth material and marked secret
- temporary HOME/global Git config path may be created
- safe.directory configuration
- optional LFS/submodule paths
- cleanup/post removes auth
- package.json declares @actions/*, uuid dependencies and ncc-generated dist build

Important: dist/index.js is a bundled generated artifact. Bundling/minification is not classified as obfuscation without additional evidence.
SOURCE_TO_DIST_REPRODUCIBILITY=TOKEN_VAZIO until build reproduction is performed.

## lukka/get-cmake
Human TypeScript source confirms:
- resolves requested version against committed catalog
- defaults CMake/Ninja request to latest
- may restore/save GitHub cloud cache
- downloads CMake and Ninja URLs through @actions/tool-cache on cache miss
- extracts archives and prepends resulting directories to PATH
- package.json declares @actions/cache/core/exec/io/tool-cache, Octokit, semver and other dependencies
- bundled dist/index.js is generated through ncc according to package script

DOWNLOAD_TO_EXECUTABLE_PATH=OBSERVED
CLOUD_CACHE_EXECUTABLE_SOURCE=OBSERVED
DOWNLOAD_INTEGRITY_CHECK_AT_THIS_LAYER=NOT_OBSERVED_IN_REVIEWED_SOURCE
DIST_REPRODUCIBILITY=TOKEN_VAZIO

## High-value route
workflow input -> action descriptor -> generated/bundled action -> human source/dependency graph -> network/cache/container -> PATH/shell/process

syslog=OBSERVE|0x0000000C|ACTIMPL-0002|implementation_tails_recorded|next=Rust_Cargo_pre_intent
