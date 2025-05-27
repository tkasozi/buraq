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

        // Clear the "Loading releases..." message
        releasesContainer.innerHTML = '';

        releases.forEach(release => {
            const releaseElement = document.createElement('div');
            releaseElement.classList.add('release');

            const releaseTitle = document.createElement('h3');
            releaseTitle.textContent = release.name || `Release ${release.tag_name}`;

            const releaseTag = document.createElement('p');
            releaseTag.innerHTML = `Version: <span class="tag-name">${release.tag_name}</span>`;

            const releaseDate = document.createElement('p');
            releaseDate.textContent = `Published: ${new Date(release.published_at).toLocaleDateString()}`;

            const assetsList = document.createElement('ul');
            if (release.assets.length > 0) {
                release.assets.forEach(asset => {
                    const assetItem = document.createElement('li');
                    const assetLink = document.createElement('a');
                    assetLink.href = asset.browser_download_url;
                    assetLink.textContent = asset.name;
                    assetLink.setAttribute('target', '_blank'); // Open in new tab
                    assetLink.setAttribute('download', ''); // Suggests to browser to download

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

            releaseElement.appendChild(releaseTitle);
            releaseElement.appendChild(releaseTag);
            releaseElement.appendChild(releaseDate);

            if (release.body) {
                const releaseNotes = document.createElement('div');
                releaseNotes.classList.add('release-notes');
                releaseNotes.innerHTML = `<p><strong>Release Notes:</strong></p><div>${formatReleaseBody(release.body)}</div>`; // Use a helper to format if needed
                releaseElement.appendChild(releaseNotes);
            }

            releaseElement.appendChild(document.createElement('h4')).textContent = 'Downloads:';
            releaseElement.appendChild(assetsList);
            releasesContainer.appendChild(releaseElement);
        });
    }

    // Basic formatter for release body (GitHub uses Markdown)
    // For a more robust solution, you might use a Markdown library
    function formatReleaseBody(body) {
        // Replace newlines with <br> for HTML display
        // This is a very basic conversion. Markdown has more features.
        return body.replace(/\r\n/g, '<br>').replace(/\n/g, '<br>');
    }

    // Fetch releases when the page loads
    if (releasesContainer) {
        fetchReleases();
    }
});
