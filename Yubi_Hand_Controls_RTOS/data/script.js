function sendValue(val) {
    fetch("/set?val=" + val);
}