document.addEventListener('DOMContentLoaded', function() {
    const releasesContainer = document.getElementById('releases-container');
    const currentYearSpan = document.getElementById('current-year');

    // Set current year in footer
    if (currentYearSpan) {
        currentYearSpan.textContent = new Date().getFullYear();
    }

    // GitHub repository details
    const owner = 'tkasozi';
    const repo = 'ITools';
    const GITHUB_API_URL = `https://api.github.com/repos/${owner}/${repo}/releases`;

    async function fetchReleases() {
        try {
            const response = await fetch(GITHUB_API_URL);
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            const releases = await response.json();
            displayReleases(releases);
        } catch (error) {
            console.error('Error fetching releases:', error);
            if (releasesContainer) {
                releasesContainer.innerHTML = '<p>Sorry, could not load releases at this time. Please try again later or visit the <a href="https://github.com/tkasozi/ITools/releases" target="_blank">GitHub releases page</a> directly.</p>';
            }
        }
    }

    function displayReleases(releases) {
        if (!releasesContainer) return;

        if (releases.length === 0) {
            releasesContainer.innerHTML = '<p>No releases found. Check the <a href="https://github.com/tkasozi/ITools/releases" target="_blank">GitHub releases page</a>.</p>';
            return;
        }

        releasesContainer.innerHTML = ''; // Clear loading message

        releases.forEach(release => {
            const releaseElement = document.createElement('div');
            releaseElement.classList.add('release');

            const releaseTitle = document.createElement('h3');
            releaseTitle.textContent = release.name || `Release ${release.tag_name}`;

            const releaseTag = document.createElement('p');
            releaseTag.innerHTML = `Version: <span class="tag-name">${release.tag_name}</span>`;

            const releaseDate = document.createElement('p');
            releaseDate.textContent = `Published: ${new Date(release.published_at).toLocaleDateString()}`;

            releaseElement.appendChild(releaseTitle);
            releaseElement.appendChild(releaseTag);
            releaseElement.appendChild(releaseDate);

            if (release.body) {
                const releaseNotesContainer = document.createElement('div');
                releaseNotesContainer.classList.add('release-notes');

                const releaseNotesTitle = document.createElement('h4'); // Changed from <p><strong>...</strong></p>
                releaseNotesTitle.textContent = 'Release Notes:';
                releaseNotesContainer.appendChild(releaseNotesTitle);

                const releaseNotesContent = document.createElement('div');
                // Use marked.parse() to convert Markdown to HTML
                releaseNotesContent.innerHTML = marked.parse(release.body);
                releaseNotesContainer.appendChild(releaseNotesContent);

                releaseElement.appendChild(releaseNotesContainer);
            }

            const downloadsTitle = document.createElement('h4');
            downloadsTitle.textContent = 'Downloads:';
            releaseElement.appendChild(downloadsTitle);

            const assetsList = document.createElement('ul');
            if (release.assets.length > 0) {
                release.assets.forEach(asset => {
                    const assetItem = document.createElement('li');
                    const assetLink = document.createElement('a');
                    assetLink.href = asset.browser_download_url;
                    assetLink.textContent = asset.name;
                    assetLink.setAttribute('target', '_blank');
                    assetLink.setAttribute('download', '');

                    const assetSize = document.createElement('span');
                    assetSize.textContent = ` (${(asset.size / (1024 * 1024)).toFixed(2)} MB)`;
                    assetSize.style.fontSize = '0.9em';
                    assetSize.style.color = '#555';

                    assetItem.appendChild(assetLink);
                    assetItem.appendChild(assetSize);
                    assetsList.appendChild(assetItem);
                });
            } else {
                const noAssetsItem = document.createElement('li');
                noAssetsItem.textContent = 'No downloadable files for this release.';
                assetsList.appendChild(noAssetsItem);
            }

            releaseElement.appendChild(assetsList);
            releasesContainer.appendChild(releaseElement);
        });
    }

    // The old formatReleaseBody function is no longer needed as marked.parse() handles Markdown conversion.
    // function formatReleaseBody(body) { ... } // REMOVE THIS FUNCTION

    if (releasesContainer) {
        fetchReleases();
    }
});