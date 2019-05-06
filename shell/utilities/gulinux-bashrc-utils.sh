BASHRC_FILE="${BASHRC_FILE:-$HOME/.bashrc}"
alias rebashrc="source $BASHRC_FILE"
alias vimbashrc="vim ${BASHRC_FILE}; source ${BASHRC_FILE}"
export EDITOR=vim
export VISUAL=vim
mkcd() {
    mkdir -p "$@"
    cd "$@"
}

mkdatedir() {
    DATESPEC="%Y-%m-%d"
    if [ "$1" == '-t' ]; then
        DATESPEC="${DATESPEC}T%H-%M-%S"
    elif [ "$1" == '-u' ]; then
        DATESPEC="%s"
    fi
    mkdir -p "$( ${DATE_COMMAND:-date} +${DATESPEC} )"
}
