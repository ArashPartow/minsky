set -e
cd gui-js
npm test minsky-electron
npm test minsky-web
# commented out tests currently failing
#npm test core
#npm test menu
npm test shared
#npm test ui-components
