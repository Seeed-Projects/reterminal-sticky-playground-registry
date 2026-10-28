# Community external projects

[中文](community-external-projects.zh-CN.md) · [Firmware contribution guide](contributing-firmware.md)

## Purpose and eligibility

An external community card opens the author's website in a new tab. Eligible
projects provide a specific, working reTerminal Sticky function: for example,
an installer that generates device-specific firmware or a browser tool that
prepares files for a supported Sticky reader. The destination opens directly
to the tool, installation flow, or project-specific instructions.

Explain why a useful `.bin` cannot be supplied and why the website is part of
the working product. For a browser conversion tool, the deliverable is the
converted document and the browser application itself; record that distinction
and name the reader firmware needed on Sticky. For a personalized installer,
describe which configuration is generated and where it is processed.

Maintainers approve a useful Sticky workflow and its destination. The summary,
delivery reason, and usage instructions explain the device benefit so reviewers
can assess whether the card provides a working tool rather than a promotional link.

## Files and metadata

Submit `firmwares/<id>/firmware.json`, `README.md`, and a real Sticky photo or
screenshot under `assets/`. Start with `catalogSection: "draft"` during review.
Publication uses `group: "community"`,
`catalogSection: "community"`, a supported `category`, and `mode: "external"`.
The external route uses `source.url`, author credit, documentation, support,
and an `external` object. The build and binary delivery fields belong to the
`flash` route. Use **Submit with the form** on the Firmware page and select
**External project**, or prepare the files and PR manually.

## Submit with the website form

1. Open the Firmware page and select **Submit with the form → External project**.
2. Fill in the project name, category, summary, source license, project page,
   and author. Upload a real Sticky photo (PNG, JPEG, or WebP, up to 5 MB).
3. Enter the exact destination and complete the application answers below.
   The external route uses these answers instead of a firmware
   version and binary upload.
4. Confirm the review requirements, complete the human check, and submit.
   The service creates a community `draft` entry containing metadata, README,
   and the photo. The PR includes the full application and maintainer checklist.
5. Open the returned PR link to follow review. A maintainer confirms the workflow
   and changes `catalogSection` to `community` when publication is approved.

`external` has `label` (1–60 characters), `url` (HTTPS), `description`
(1–400 characters), and the following `submission` object. Delivery reason and
workflow use 20–2000 characters. Usage conditions use 1–2000 characters; a
short answer such as "None" is valid when no requirements apply.

| Field | What the applicant provides |
| --- | --- |
| `hostingReason` | Why a usable `.bin` cannot be supplied and why external delivery is necessary |
| `userFlow` | Reproducible steps, required firmware/accessories, inputs, outputs, and expected device behavior |
| `siteRequirements` | Accounts, charges, browser/device permissions, software installation, collected/transmitted data, and third-party services; state explicitly which are unnecessary |

The README explains the same workflow, support contacts, exact destination,
and usage conditions in readable form. Put the complete explanation in
the PR's **External community project application** section as well. Application
metadata remains available to reviewers when a later update changes the entry.

## Review and publication

1. Run `npm test` and `npm run validate` with Node.js 20 or newer. Validation
   checks metadata, the complete application, HTTPS links, and local assets.
2. The review comment shows the destination and domain, delivery reason,
   usage conditions, preview, and link results.
   Changes to an existing destination are highlighted with the previous URL.
   External redirects are reported for inspection rather than followed by the
   automated checker. HTTP success establishes reachability only.
3. A maintainer opens the full user flow, inspects redirects and downloads,
   and checks requested permissions and data handling.
   Account or paid steps must be described sufficiently for informed review.
4. Approve the documented project and destination together. Publish by changing
   `catalogSection` to `community` after review is complete.
   The website displays an external-site badge and the destination domain;
   the primary button opens the reviewed URL. Install counts apply to installs
   completed through the Sticky website.

## Updates and continued eligibility

Changes to the URL, destination owner, product purpose, permissions, payments,
or data handling go through a new PR with an updated application.
For an external-to-flash transition, provide the complete tested package and
use the corresponding contribution checklist.

The author maintains the service and a reachable support contact. Maintainers
can return a card to `draft` while investigating an unavailable destination,
unexpected downloads, suspected compromise, promotional repurposing, or a
material change to the reviewed workflow. Publication resumes after review.
